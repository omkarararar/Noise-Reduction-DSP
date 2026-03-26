/*
 * Real-time MMSE LMS Adaptive Line Enhancer (ALE) for single-mic.
 * Uses a delayed copy of the signal to predict the true periodic component,
 * wiping out uncorrelated white noise.
 */
#define FILTER_LENGTH 32
#define DELAY_SAMPLES 5

float w[FILTER_LENGTH] = {0};
float x_buffer[FILTER_LENGTH] = {0};
float delay_buffer[DELAY_SAMPLES] = {0};

float mu = 0.001f; // Learning rate (Keep small to prevent instability)
int delay_idx = 0;

void setup() {
    Serial.begin(115200);
    
    // Initialize delay buffer
    int val = analogRead(A0);
    for(int i=0; i<DELAY_SAMPLES; i++) {
        delay_buffer[i] = (float)val;
    }
}

void loop() {
    // 1. Read primary signal
    float d = (float)analogRead(A0);
    
    // 2. Get the delayed sample (our synthetic 'reference')
    float x_in = delay_buffer[delay_idx];
    
    // 3. Put current sample into delay buffer for future use
    delay_buffer[delay_idx] = d;
    delay_idx = (delay_idx + 1) % DELAY_SAMPLES;
    
    // 4. Shift FIR filter buffer
    for (int j = FILTER_LENGTH - 1; j > 0; j--) {
        x_buffer[j] = x_buffer[j - 1];
    }
    x_buffer[0] = x_in;
    
    // 5. Compute filter prediction (y)
    float y = 0.0f;
    for (int j = 0; j < FILTER_LENGTH; j++) {
        y += w[j] * x_buffer[j];
    }
    
    // 6. Calculate error and adapt weights
    float e = d - y;
    for (int j = 0; j < FILTER_LENGTH; j++) {
        w[j] = w[j] + 2.0f * mu * e * x_buffer[j];
    }
    
    // In an ALE, the prediction 'y' IS the denoised periodic signal
    Serial.print("Raw:");
    Serial.print(d);
    Serial.print(",");
    Serial.print("Filtered:");
    Serial.println(y);
    
    delay(2);
}
