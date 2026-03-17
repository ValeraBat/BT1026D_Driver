#include <Arduino.h>
// If you are using an AVR (Uno/Nano) with SoftwareSerial, uncomment the next line
// #include <SoftwareSerial.h>
#include "UniversalBT1026.h"

UniversalBT1026 bt;

// For AVR with SoftwareSerial:
// SoftwareSerial btSerial(10, 11);

// For ESP32 or chips with a second hardware serial port:
HardwareSerial& btSerial = Serial1;

void btStateChanged(UniversalBT1026::BTState state) {
    if (state == UniversalBT1026::PLAYING) {
        Serial.println("Audio is now PLAYING.");
    } else if (state == UniversalBT1026::CONNECTED) {
        Serial.println("Device CONNECTED/PAUSED.");
    }
}

void btLog(const char* msg) {
    Serial.print("SYS_LOG: ");
    Serial.println(msg);
}

void setup() {
    Serial.begin(115200);
    // Be careful, on some ESP32 boards Serial1 needs to be specifically configured with RX/TX pins.
    // Example for ESP32: btSerial.begin(115200, SERIAL_8N1, 16, 17);
    btSerial.begin(115200); 

    Serial.println("Starting Universal BT1026 Driver...");

    bt.onStateChanged(btStateChanged);
    bt.onLog(btLog);

    bt.begin(&btSerial);
}

void loop() {
    bt.loop();
    
    if (Serial.available()) {
        char c = Serial.read();
        if (c == 'p') bt.play();
        if (c == 's') bt.pause();
        if (c == 'n') bt.next();
        if (c == 'b') bt.prev();
    }
}
