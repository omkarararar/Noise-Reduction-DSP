/*
 * Real-time LMS Active Noise Cancellation (ANC)
 * 
 * NOTE: THIS REQUIRES TWO MICROPHONES!
 * A0: Primary Mic (Speech + Noise)
 * A1: Reference Mic (Pure Noise Source)
 */
#define FILTER_LENGTH 16

float w[FILTER_LENGTH] = {0};
float x_buffer[FILTER_LENGTH] = {0};

float mu = 0.01f; // Learning rate

void setup() {
    Serial.begin(115200);
}

void loop() {
    // 1. Read from both microphones
    float d = (float)analogRead(A0); // Primary (speech + noise)
    float x_in = (float)analogRead(A1); // Reference (just noise)
    
    // 2. Shift the reference buffer
    for (int j = FILTER_LENGTH - 1; j > 0; j--) {
        x_buffer[j] = x_buffer[j - 1];
    }
    x_buffer[0] = x_in;
    
    // 3. Predict the noise in the primary mic
    float y = 0.0f;
    for (int j = 0; j < FILTER_LENGTH; j++) {
        y += w[j] * x_buffer[j];
    }
    
    // 4. Subtract predicted noise from primary mic signal
    float e = d - y;
    
    // 5. Adapt weights based on error
    for (int j = 0; j < FILTER_LENGTH; j++) {
        w[j] = w[j] + 2.0f * mu * e * x_buffer[j];
    }
    
    // In dual-mic ANC, the 'error' (e) is the clean speech!
    Serial.print("Primary_Noisy:");
    Serial.print(d);
    Serial.print(",");
    Serial.print("Clean_Denoised:");
    Serial.println(e);
    
    delay(2);
}
