#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Applies a simple 1D Kalman filter to the input signal.
 * 
 * @param input  Input signal array
 * @param output Array to store the filtered output
 * @param n      Number of samples
 * @param q      Process noise covariance (how much we expect the true state to change)
 * @param r      Measurement noise covariance (how noisy our measurements are)
 */
void kalman_filter(const float* input, float* output, int n, float q, float r) {
    if (n <= 0) return;
    
    // Initial state guess
    float x = input[0];
    float p = 1.0f; // Initial error covariance
    
    for (int i = 0; i < n; i++) {
        // 1. Predict
        float x_pred = x;      // We assume the state doesn't change much (random walk)
        float p_pred = p + q;  // Error covariance increases with process noise
        
        // 2. Update
        // Kalman Gain
        float k = p_pred / (p_pred + r);
        
        // Update estimate with measurement
        float z = input[i];
        x = x_pred + k * (z - x_pred);
        
        // Update error covariance
        p = (1.0f - k) * p_pred;
        
        // Store output
        output[i] = x;
    }
}

#define MAX_SAMPLES 10000

int main(int argc, char* argv[]) {
    // Tuning parameters
    float q = 0.01f; // Default process noise
    float r = 1.0f;  // Default measurement noise
    
    if (argc >= 3) {
        q = (float)atof(argv[1]);
        r = (float)atof(argv[2]);
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
    printf("Applying Kalman filter with Q=%.4f, R=%.4f\n", q, r);
    
    float* kalman_less_noisy = (float*)malloc(num_samples * sizeof(float));
    float* kalman_noisy = (float*)malloc(num_samples * sizeof(float));
    
    if (!kalman_less_noisy || !kalman_noisy) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Apply filters. For "less noisy", the measurement variance is roughly 0.1
    // For "noisy", the measurement variance is roughly 1.0 (based on sine_wave.c)
    kalman_filter(less_noisy, kalman_less_noisy, num_samples, q, 0.1f);
    kalman_filter(noisy, kalman_noisy, num_samples, q, 1.0f);
    
    // Write out results
    FILE* out_fp = fopen("kalman_filtered_waveforms.csv", "w");
    if (!out_fp) {
        printf("Could not open kalman_filtered_waveforms.csv for writing.\n");
        return 1;
    }
    
    fprintf(out_fp, "Time,Clean,LessNoisy,Noisy,KalmanLessNoisy,KalmanNoisy\n");
    for (int i = 0; i < num_samples; i++) {
        fprintf(out_fp, "%f,%f,%f,%f,%f,%f\n", 
            times[i], clean[i], less_noisy[i], noisy[i], 
            kalman_less_noisy[i], kalman_noisy[i]);
    }
    fclose(out_fp);
    
    printf("Filtering complete. Results saved to kalman_filtered_waveforms.csv\n");
    
    free(kalman_less_noisy);
    free(kalman_noisy);
    
    return 0;
}
