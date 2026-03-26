/*
 * Real-time 1D Wiener Filter for Arduino
 * Uses a running circular buffer to estimate local mean and variance.
 * NOTE: Output is delayed by (WINDOW_SIZE / 2) effectively, but here we process causally.
 */
#define WINDOW_SIZE 5
#define NOISE_VAR   10.0f  // Tune this value! It's the expected variance of the silence/noise

float window[WINDOW_SIZE] = {0};
int win_idx = 0;

void setup() {
    Serial.begin(115200);
    // Optional: wait for serial port to connect
    // while (!Serial);
    
    // Fill buffer initially to avoid startup glitches
    int initial_val = analogRead(A0);
    for(int i=0; i<WINDOW_SIZE; i++) {
        window[i] = (float)initial_val;
    }
}

void loop() {
    // 1. Read single sample
    int raw_val = analogRead(A0);
    float val = (float)raw_val;
    
    // 2. Add to circular buffer
    window[win_idx] = val;
    win_idx = (win_idx + 1) % WINDOW_SIZE;
    
    // 3. Compute local mean
    float sum = 0.0f;
    for(int i=0; i<WINDOW_SIZE; i++) {
        sum += window[i];
    }
    float local_mean = sum / WINDOW_SIZE;
    
    // 4. Compute local variance
    float var_sum = 0.0f;
    for(int i=0; i<WINDOW_SIZE; i++) {
        var_sum += (window[i] - local_mean) * (window[i] - local_mean);
    }
    float local_variance = var_sum / WINDOW_SIZE;
    
    // 5. Apply Wiener equation
    float filtered_val = local_mean;
    if (local_variance > NOISE_VAR) {
        filtered_val = local_mean + ((local_variance - NOISE_VAR) / local_variance) * (val - local_mean);
    }
    
    // 6. Output to Serial Plotter
    Serial.print("Raw:");
    Serial.print(val);
    Serial.print(",");
    Serial.print("Filtered:");
    Serial.println(filtered_val);
    
    // Sampling rate control (e.g. 500 Hz -> 2ms)
    delay(2);
}
