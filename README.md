# BT1026 Driver for ESP32

An asynchronous, FreeRTOS-based driver for the Feasycom FSC-BT1026X Bluetooth audio modules (BT1026A, BT1026C, BT1026D, BT1026E, etc.). This library uses an event-driven architecture, avoiding blocking `delay()` calls, making it ideal for high-performance firmware running on ESP32 microcontrollers.

## Features
- **Non-blocking UART parser**: Handles incoming serial data byte-by-byte in real-time.
- **Queue-based AT Commands**: Uses FreeRTOS queues `xQueue` to stack AT commands safely without overloading the module.
- **Automatic State Machine**: Tracks connection statuses automatically via Unsolicited Result Codes (URCs) (`+A2DPSTAT`, `+HFPSTAT`, `+PLAYSTAT`).
- **Callback Architecture**: Emits events (logs, state changes, track metadata) seamlessly to your main app via C++ callbacks.
- **Full AV/HFP Support**: Play/Pause, Next/Prev, Volume Control, Caller ID extraction, Answer/Reject calls.

## Hardware Setup
You need an ESP32 connected to the BT1026 module via Hardware UART.
- ESP32 TX -> BT1026 RX
- ESP32 RX -> BT1026 TX
- Ensure solid GND connection between devices.

## Quick Start

Check the `examples/Basic_Usage` folder for a full working example.

```cpp
#include <Arduino.h>
#include <BT1026D_driver.h>

// Initialize driver with Serial2
BT1026D btDriver(Serial2);

void onStateChanged(BTConnState newState, BTConnState oldState) {
    if (newState == BTConnState::PLAYING) {
        Serial.println("Dashboard: Music started playing!");
    }
}

void setup() {
    Serial.begin(115200);
    
    // Register callback
    btDriver.setStateChangeCallback(onStateChanged);
    
    // Init communication with BT Module (ESP32 pins: RX=16, TX=17)
    btDriver.begin(115200, 16, 17);
    
    // Request initial state synchronization
    btDriver.enqueueCommand(BTCmdType::STAT);
}

void loop() {
    // The driver runs automatically in a background FreeRTOS task.
    // You can send commands safely from your main loop or interrupts.
    
    if (Serial.available()) {
        char c = Serial.read();
        if (c == 'p') {
            btDriver.enqueueCommand(BTCmdType::PLAYPAUSE);
        } else if (c == 'n') {
            btDriver.enqueueCommand(BTCmdType::FORWARD);
        }
    }
}
```