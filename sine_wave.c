#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Generates a sine wave array dynamically allocated with calloc.
 * 
 * @param frequency     Frequency of the sine wave in Hertz
 * @param amplitude     Peak amplitude of the sine wave
 * @param sampling_rate Number of samples per second
 * @param num_samples   Total number of samples to generate
 * @return              Pointer to the allocated float array, or NULL if allocation fails
 */
float* generate_sine_wave(float frequency, float amplitude, float sampling_rate, int num_samples) {
    // Dynamically allocate array for the sine wave using calloc
    float *wave = (float*)calloc(num_samples, sizeof(float));
    
    if (wave == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    for (int i = 0; i < num_samples; i++) {
        // Calculate the time at the current sample index
        float t = (float)i / sampling_rate;
        
        // Calculate the sine value at time t: amplitude * sin(2 * pi * frequency * t)
        wave[i] = amplitude * sinf(2.0f * (float)M_PI * frequency * t);
    }

    return wave;
}

/**
 * Generates a standard normally distributed random number (mean 0, variance 1)
 * using the Box-Muller transform.
 * 
 * @return Standard normal random variable
 */
float generate_standard_normal() {
    float u1 = (float)rand() / RAND_MAX;
    float u2 = (float)rand() / RAND_MAX;
    
    // To prevent log(0)
    if (u1 <= 0.0f) u1 = 1e-7f; 

    float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * (float)M_PI * u2);
    return z0;
}

/**
 * Adds Gaussian noise to a given float array.
 * 
 * @param array       Pointer to the float array to modify
 * @param num_samples Number of elements in the array
 * @param variance    Variance of the Gaussian noise
 */
void add_gaussian_noise(float* array, int num_samples, float variance) {
    if (array == NULL || num_samples <= 0 || variance < 0.0f) return;

    // Standard deviation is the square root of variance
    float std_dev = sqrtf(variance);

    for (int i = 0; i < num_samples; i++) {
        // Scale standard normal variable by standard deviation (mean remains 0)
        float noise = generate_standard_normal() * std_dev;
        array[i] += noise;
    }
}

int main(int argc, char *argv[]) {
    float frequency = 5.0f;       // 5 Hz
    float amplitude = 2.5f;       // Peak amplitude
    float sampling_rate = 100.0f; // 100 samples per second
    int num_samples = 100;        // Generate 1.0 seconds of data

    // Parse command-line arguments if provided
    if (argc >= 2) frequency = (float)atof(argv[1]);
    if (argc >= 3) amplitude = (float)atof(argv[2]);
    if (argc >= 4) sampling_rate = (float)atof(argv[3]);
    if (argc >= 5) num_samples = atoi(argv[4]);
    
    printf("Parameters:\n");
    printf("  Frequency     : %.2f Hz\n", frequency);
    printf("  Amplitude     : %.2f\n", amplitude);
    printf("  Sampling Rate : %.2f Hz\n", sampling_rate);
    printf("  Total Samples : %d\n\n", num_samples);

    float *clean_wave = generate_sine_wave(frequency, amplitude, sampling_rate, num_samples);
    float *less_noisy_wave = generate_sine_wave(frequency, amplitude, sampling_rate, num_samples);
    float *noisy_wave = generate_sine_wave(frequency, amplitude, sampling_rate, num_samples);

    if (clean_wave != NULL && less_noisy_wave != NULL && noisy_wave != NULL) {
        // Seed the random number generator
        srand((unsigned int)time(NULL)); 
        
        // Add varying levels of noise
        add_gaussian_noise(less_noisy_wave, num_samples, 0.1f);
        add_gaussian_noise(noisy_wave, num_samples, 1.0f);
        
        // Write to CSV file
        FILE *fp = fopen("waveforms.csv", "w");
        if (fp != NULL) {
            fprintf(fp, "Time,Clean,LessNoisy,Noisy\n");
            for (int i = 0; i < num_samples; i++) {
                float t = (float)i / sampling_rate;
                fprintf(fp, "%f,%f,%f,%f\n", t, clean_wave[i], less_noisy_wave[i], noisy_wave[i]);
            }
            fclose(fp);
            printf("Waveforms successfully written to waveforms.csv\n");
        } else {
            fprintf(stderr, "Failed to open waveforms.csv for writing.\n");
        }

        // Always free dynamically allocated memory once done
        free(clean_wave);
        free(less_noisy_wave);
        free(noisy_wave);
    }

    return 0;
}
