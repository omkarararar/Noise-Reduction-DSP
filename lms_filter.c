#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Applies a true LMS (Least Mean Squares) filter for Active Noise Cancellation (ANC).
 * This simulates a dual-microphone setup:
 *  1. Desired Signal (d): The Primary Microphone (Clean Speech + Noise)
 *  2. Reference Signal (x): The Noise Reference Microphone (Just Noise)
 *
 * The LMS filter tries to adapt its weights to transform the Reference Noise 
 * so it perfectly matches the noise in the Primary Microphone. 
 * The Error signal (e = d - y) becomes the clean, denoised audio!
 *
 * @param primary_mic    The noisy signal (speech + noise)
 * @param reference_mic  The correlated noise reference
 * @param clean_output   Array to store the denoised output (the error signal)
 * @param n              Number of samples
 * @param filter_length  Number of weights (taps) in the adaptive filter
 * @param mu             Learning rate / step size parameter
 */
void lms_anc_filter(const float* primary_mic, const float* reference_mic, float* clean_output, int n, int filter_length, float mu) {
    if (n <= 0 || filter_length <= 0) return;
    
    float* w = (float*)calloc(filter_length, sizeof(float));
    float* x_buffer = (float*)calloc(filter_length, sizeof(float));
    
    if (!w || !x_buffer) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    for (int i = 0; i < n; i++) {
        float d = primary_mic[i];   // Our desired signal is what the main mic hears
        float x_in = reference_mic[i]; // Our reference is the isolated noise
        
        // Shift buffer to the right
        for (int j = filter_length - 1; j > 0; j--) {
            x_buffer[j] = x_buffer[j - 1];
        }
        x_buffer[0] = x_in;
        
        // Output of the filter: The predicted noise in the primary mic!
        float y = 0.0f;
        for (int j = 0; j < filter_length; j++) {
            y += w[j] * x_buffer[j];
        }
        
        // The Error is what's left over when you subtract predicted noise from main mic.
        // In ANC, the error IS the clean signal.
        float e = d - y;
        
        // Update the filter weights using the LMS algorithm
        for (int j = 0; j < filter_length; j++) {
            w[j] = w[j] + 2.0f * mu * e * x_buffer[j];
        }
        
        // Save the "error" as our denoised clean output
        clean_output[i] = e;
    }
    
    free(w);
    free(x_buffer);
}

#define MAX_SAMPLES 10000

int main(int argc, char* argv[]) {
    // Tuning parameters
    int filter_length = 16;
    float mu = 0.01f;      // Learning rate
    
    if (argc >= 3) {
        filter_length = atoi(argv[1]);
        mu = (float)atof(argv[2]);
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
    printf("Simulating Dual-Microphone Active Noise Cancellation (ANC)\n");
    printf("Applying LMS filter with taps=%d, mu=%f\n", filter_length, mu);
    
    float* ref_noise_less = (float*)malloc(num_samples * sizeof(float));
    float* ref_noise_heavy = (float*)malloc(num_samples * sizeof(float));
    float* lms_clean_less = (float*)malloc(num_samples * sizeof(float));
    float* lms_clean_heavy = (float*)malloc(num_samples * sizeof(float));
    
    if (!ref_noise_less || !ref_noise_heavy || !lms_clean_less || !lms_clean_heavy) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Step 1: Simulate what the completely isolated "Noise Reference Microphone" hears.
    // In our synthetic data, noise = noisy_signal - clean_signal
    for (int i = 0; i < num_samples; i++) {
        ref_noise_less[i] = less_noisy[i] - clean[i];
        ref_noise_heavy[i] = noisy[i] - clean[i];
    }
    
    // Step 2: Pass both the Primary Mic (noisy signal) and Reference Mic (isolated noise) into the LMS filter
    lms_anc_filter(less_noisy, ref_noise_less, lms_clean_less, num_samples, filter_length, mu);
    lms_anc_filter(noisy, ref_noise_heavy, lms_clean_heavy, num_samples, filter_length, mu);
    
    // Write out results
    FILE* out_fp = fopen("lms_filtered_waveforms.csv", "w");
    if (!out_fp) {
        printf("Could not open lms_filtered_waveforms.csv for writing.\n");
        return 1;
    }
    
    fprintf(out_fp, "Time,Clean,LessNoisy,Noisy,LMSCleanLess,LMSCleanHeavy\n");
    for (int i = 0; i < num_samples; i++) {
        fprintf(out_fp, "%f,%f,%f,%f,%f,%f\n", 
            times[i], clean[i], less_noisy[i], noisy[i], 
            lms_clean_less[i], lms_clean_heavy[i]);
    }
    fclose(out_fp);
    
    printf("Filtering complete. Results saved to lms_filtered_waveforms.csv\n");
    
    free(ref_noise_less);
    free(ref_noise_heavy);
    free(lms_clean_less);
    free(lms_clean_heavy);
    
    return 0;
}
