#include <Adafruit_NeoPixel.h>

// Pin definition
#define LED_PIN 2
#define MATRIX_SIZE 64  
#define BRIGHTNESS 50  

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(MATRIX_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(9600);
    matrix.begin();
    matrix.setBrightness(BRIGHTNESS);
    matrix.show();}

void loop() {
    if (Serial.available() > 0) {
        String receivedData = Serial.readStringUntil('\n');
        int x, y;
        char hand[10];
        // Parsing the received data
        sscanf(receivedData.c_str(), "%d,%d,%s", &x, &y, hand);

        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            int ledIndex = y * 8 + x;  // Convert (x,y) to single LED index
            
            // Determine color based on hand
            if (strcmp(hand, "Left") == 0) {
                matrix.setPixelColor(ledIndex, matrix.Color(255, 0, 0));} // Red for Left hand
            else if (strcmp(hand, "Right") == 0) {
                matrix.setPixelColor(ledIndex, matrix.Color(0, 0, 255));} // Blue for Right hand
            matrix.show(); } // Refreshing the LED
    }
}