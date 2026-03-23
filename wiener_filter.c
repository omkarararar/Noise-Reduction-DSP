#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Applies a 1D Wiener filter to the input signal.
 * 
 * @param input       Input signal array
 * @param output      Array to store the filtered output
 * @param n           Number of samples
 * @param window_size Size of the local variance neighborhood
 */
void wiener_filter(const float* input, float* output, int n, int window_size) {
    if (n <= 0 || window_size <= 0) return;
    
    float* local_mean = (float*)malloc(n * sizeof(float));
    float* local_var = (float*)malloc(n * sizeof(float));
    
    if (!local_mean || !local_var) {
        printf("Memory allocation failed!\n");
        return;
    }

    int half_window = window_size / 2;
    float noise_var = 0.0f;
    
    // 1. Calculate the local mean and local variance
    for (int i = 0; i < n; i++) {
        int start = i - half_window;
        int end = i + half_window;
        
        // Handle boundary conditions
        if (start < 0) start = 0;
        if (end >= n) end = n - 1;
        
        int count = end - start + 1;
        
        // Calculate mean
        float sum = 0.0f;
        for (int j = start; j <= end; j++) {
            sum += input[j];
        }
        local_mean[i] = sum / count;
        
        // Calculate variance
        float var_sum = 0.0f;
        for (int j = start; j <= end; j++) {
            var_sum += (input[j] - local_mean[i]) * (input[j] - local_mean[i]);
        }
        local_var[i] = (count > 1) ? (var_sum / count) : 0.0f;
        
        noise_var += local_var[i];
    }
    
    // 2. Estimate noise variance as the average of all local variances
    noise_var /= n;
    
    // 3. Apply the Wiener filter formula
    for (int i = 0; i < n; i++) {
        if (local_var[i] < noise_var) {
            output[i] = local_mean[i];
        } else {
            output[i] = local_mean[i] + ((local_var[i] - noise_var) / local_var[i]) * (input[i] - local_mean[i]);
        }
    }
    
    free(local_mean);
    free(local_var);
}

#define MAX_SAMPLES 10000

int main(int argc, char* argv[]) {
    // Read window size from arguments if available
    int window_size = 5; 
    if (argc >= 2) {
        window_size = atoi(argv[1]);
        if (window_size <= 0) window_size = 5;
    }

    FILE* fp = fopen("waveforms.csv", "r");
    if (!fp) {
        printf("Error: Could not open waveforms.csv for reading.\n");
        printf("Please run the sine_wave generation first to create this file.\n");
        return 1;
    }
    
    float times[MAX_SAMPLES];
    float clean[MAX_SAMPLES];
    float less_noisy[MAX_SAMPLES];
    float noisy[MAX_SAMPLES];
    
    char line[256];
    
    // Skip the header
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("Error reading waveforms.csv header.\n");
        fclose(fp);
        return 1;
    }
    
    int num_samples = 0;
    while(fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%f,%f,%f,%f", &times[num_samples], &clean[num_samples], &less_noisy[num_samples], &noisy[num_samples]) == 4) {
            num_samples++;
            if (num_samples >= MAX_SAMPLES) {
                printf("Warning: Hit maximum sample limit of %d\n", MAX_SAMPLES);
                break;
            }
        }
    }
    fclose(fp);
    
    printf("Loaded %d samples from waveforms.csv\n", num_samples);
    printf("Applying Wiener filter with window size: %d\n", window_size);
    
    float* filtered_less_noisy = (float*)malloc(num_samples * sizeof(float));
    float* filtered_noisy = (float*)malloc(num_samples * sizeof(float));
    
    if (!filtered_less_noisy || !filtered_noisy) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Apply filters
    wiener_filter(less_noisy, filtered_less_noisy, num_samples, window_size);
    wiener_filter(noisy, filtered_noisy, num_samples, window_size);
    
    // Write out results
    FILE* out_fp = fopen("filtered_waveforms.csv", "w");
    if (!out_fp) {
        printf("Could not open filtered_waveforms.csv for writing.\n");
        return 1;
    }
    
    fprintf(out_fp, "Time,Clean,LessNoisy,Noisy,FilteredLessNoisy,FilteredNoisy\n");
    for (int i = 0; i < num_samples; i++) {
        fprintf(out_fp, "%f,%f,%f,%f,%f,%f\n", 
            times[i], clean[i], less_noisy[i], noisy[i], 
            filtered_less_noisy[i], filtered_noisy[i]);
    }
    fclose(out_fp);
    
    printf("Filtering complete. Results saved to filtered_waveforms.csv\n");
    
    free(filtered_less_noisy);
    free(filtered_noisy);
    
    return 0;
}
