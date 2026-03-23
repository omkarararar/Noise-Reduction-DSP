#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Applies an LMS (Least Mean Squares) Adaptive Line Enhancer (ALE)
 * to minimize the Mean Squared Error (MMSE approach).
 * 
 * An ALE uses a delayed version of the noisy signal to predict the current value.
 * Since white noise is uncorrelated with its delayed self, but the sine wave is correlated,
 * the filter learns to pass the sine wave and reject the noise.
 * 
 * @param input          Input noisy signal array
 * @param output         Array to store the filtered output
 * @param n              Number of samples
 * @param filter_length  Number of weights in the adaptive filter
 * @param mu             Step size / learning rate parameter
 * @param delay          Prediction delay (samples)
 */
void mmse_lms_filter(const float* input, float* output, int n, int filter_length, float mu, int delay) {
    if (n <= 0 || filter_length <= 0) return;
    
    float* w = (float*)calloc(filter_length, sizeof(float));
    float* x_buffer = (float*)calloc(filter_length, sizeof(float));
    
    if (!w || !x_buffer) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    for (int i = 0; i < n; i++) {
        // Desired signal is the current noisy measurement
        float d = input[i];
        
        // Filter input is the delayed noisy measurement
        float x_in = (i >= delay) ? input[i - delay] : 0.0f;
        
        // Shift buffer to the right
        for (int j = filter_length - 1; j > 0; j--) {
            x_buffer[j] = x_buffer[j - 1];
        }
        x_buffer[0] = x_in;
        
        // Calculate filter output (the MMSE prediction of the clean signal)
        float y = 0.0f;
        for (int j = 0; j < filter_length; j++) {
            y += w[j] * x_buffer[j];
        }
        
        // Calculate error
        float e = d - y;
        
        // Update filter weights strictly using the LMS adaptation rule
        for (int j = 0; j < filter_length; j++) {
            w[j] = w[j] + 2.0f * mu * e * x_buffer[j];
        }
        
        // The output is our predicted denoised signal
        output[i] = y;
    }
    
    free(w);
    free(x_buffer);
}

#define MAX_SAMPLES 10000

int main(int argc, char* argv[]) {
    // Tuning parameters
    int filter_length = 32;
    float mu = 0.001f;      // Learning rate
    int delay = 5;          // Delay in samples
    
    if (argc >= 4) {
        filter_length = atoi(argv[1]);
        mu = (float)atof(argv[2]);
        delay = atoi(argv[3]);
    }

    FILE* fp = fopen("waveforms.csv", "r");
    if (!fp) {
        printf("Error: Could not open waveforms.csv for reading.\n");
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
            if (num_samples >= MAX_SAMPLES) break;
        }
    }
    fclose(fp);
    
    printf("Loaded %d samples from waveforms.csv\n", num_samples);
    printf("Applying MMSE (LMS ALE) filter with length=%d, mu=%f, delay=%d\n", filter_length, mu, delay);
    
    float* mmse_less_noisy = (float*)malloc(num_samples * sizeof(float));
    float* mmse_noisy = (float*)malloc(num_samples * sizeof(float));
    
    if (!mmse_less_noisy || !mmse_noisy) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Process both signals
    // For "less noisy", a slightly larger step size might converge faster without instability,
    // but we can use the same generic parameters for both as a demonstration.
    mmse_lms_filter(less_noisy, mmse_less_noisy, num_samples, filter_length, mu * 5.0f, delay); 
    mmse_lms_filter(noisy, mmse_noisy, num_samples, filter_length, mu, delay);
    
    // Write out results
    FILE* out_fp = fopen("mmse_filtered_waveforms.csv", "w");
    if (!out_fp) {
        printf("Could not open mmse_filtered_waveforms.csv for writing.\n");
        return 1;
    }
    
    fprintf(out_fp, "Time,Clean,LessNoisy,Noisy,MMSELessNoisy,MMSENoisy\n");
    for (int i = 0; i < num_samples; i++) {
        fprintf(out_fp, "%f,%f,%f,%f,%f,%f\n", 
            times[i], clean[i], less_noisy[i], noisy[i], 
            mmse_less_noisy[i], mmse_noisy[i]);
    }
    fclose(out_fp);
    
    printf("Filtering complete. Results saved to mmse_filtered_waveforms.csv\n");
    
    free(mmse_less_noisy);
    free(mmse_noisy);
    
    return 0;
}
