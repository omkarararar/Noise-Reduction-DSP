#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

int main() {
    float frequency = 5.0f;       // 5 Hz
    float amplitude = 2.5f;       // Peak amplitude
    float sampling_rate = 100.0f; // 100 samples per second
    int num_samples = 50;         // Generate 0.5 seconds of data

    float *sine_wave = generate_sine_wave(frequency, amplitude, sampling_rate, num_samples);

    if (sine_wave != NULL) {
        printf("Generated Sine Wave (%d samples):\n", num_samples);
        for (int i = 0; i < num_samples; i++) {
            printf("Sample %2d: %8.4f\n", i, sine_wave[i]);
        }
        
        // Always free dynamically allocated memory once done
        free(sine_wave);
    }

    return 0;
}
