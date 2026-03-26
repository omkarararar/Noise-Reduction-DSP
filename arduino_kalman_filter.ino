/*
 * Real-time 1D Kalman Filter for Arduino
 * Extremely fast, no arrays needed. Keeps track of state dynamically.
 */
// Tuning parameters
float q = 0.01f; // Process noise covariance (Expected change in signal)
float r = 1.0f;  // Measurement noise covariance (Assumed noise in ADC/Sensor)

// State variables
float x = 0.0f;  // Current estimated signal
float p = 1.0f;  // Current error covariance

void setup() {
    Serial.begin(115200);
    
    // Initialize state with first reading
    x = (float)analogRead(A0);
}

void loop() {
    // 1. Read single measurement
    int raw_val = analogRead(A0);
    float z = (float)raw_val;
    
    // 2. Predict step
    float x_pred = x;      // Assume value stays same (random walk model)
    float p_pred = p + q;  // Error increases over time
    
    // 3. Update step
    float k = p_pred / (p_pred + r); // Kalman gain
    x = x_pred + k * (z - x_pred);   // Bring estimate closer to measurement
    p = (1.0f - k) * p_pred;         // Update error
    
    // 4. Output to Serial Plotter
    Serial.print("Raw:");
    Serial.print(z);
    Serial.print(",");
    Serial.print("Filtered:");
    Serial.println(x);
    
    // Sampling rate control
    delay(2);
}
