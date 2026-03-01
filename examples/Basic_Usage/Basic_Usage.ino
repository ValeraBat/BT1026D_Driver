#include <Arduino.h>
#include <BT1026D_driver.h>

// Use Hardware Serial 2 for the Bluetooth module
BT1026D btDriver(Serial2);

// Callback function to handle module logs
void onBtLog(const char* msg) {
    Serial.println(msg); // Forward BT logs to PC Serial Monitor
}

// Callback function to handle connection state changes
void onBtStateChange(BTConnState newState, BTConnState oldState) {
    Serial.printf("\n[EVENT] BT State changed from %d to %d\n", (int)oldState, (int)newState);
    
    if (newState == BTConnState::PLAYING) {
        Serial.println(">>> Music is currently playing");
    } else if (newState == BTConnState::CALL_INCOMING) {
        Serial.println(">>> BRING RING! Incoming call!");
    }
}

// Callback function to handle metadata (Songs, Caller ID)
void onBtMetadata(const char* type, const char* data) {
    Serial.printf("\n[META] %s: %s\n", type, data);
}

void setup() {
    // Initialize PC Serial Monitor
    Serial.begin(115200);
    delay(1000);
    Serial.println("BT1026D Basic Example Started.");

    // 1. Assign Callbacks
    btDriver.onLog(onBtLog);
    btDriver.onStateChange(onBtStateChange);
    btDriver.onMetadata(onBtMetadata);

    // 2. Start driver (Hardware UART2: RX=16, TX=17)
    if (!btDriver.begin(115200, 16, 17)) {
        Serial.println("Failed to start BT driver context.");
        while(1); 
    }

    // 3. Queue initialization commands
    Serial.println("Syncing state with BT module...");
    btDriver.enqueueCommand(BTCmdType::STAT); // Request connection status of all profiles
}

void loop() {
    // Process input from PC Serial console to test sending commands to phone
    if (Serial.available()) {
        char c = Serial.read();
        
        switch (c) {
            case 'p':
                Serial.println("Command: Play/Pause");
                btDriver.enqueueCommand(BTCmdType::PLAYPAUSE);
                break;
            case 'n':
                Serial.println("Command: Next Track");
                btDriver.enqueueCommand(BTCmdType::FORWARD);
                break;
            case 'b':
                Serial.println("Command: Previous Track");
                btDriver.enqueueCommand(BTCmdType::BACKWARD);
                break;
            case '+':
                Serial.println("Command: Volume +");
                btDriver.enqueueCommand(BTCmdType::SPKVOL, 10);
                break;
            case 'a':
                Serial.println("Command: Answer Call");
                btDriver.enqueueCommand(BTCmdType::HFPANSW);
                break;
            case 'h':
                Serial.println("Command: Hang Up / Reject Call");
                btDriver.enqueueCommand(BTCmdType::HFPCHUP);
                break;
        }
    }
}