# Universal_BT1026 Driver

A lightweight, non-RTOS C++ driver for QCC3034/QCC5125 (BT1026C/D, BT1036) Bluetooth modules. Designed with the **Superloop** architecture, making it highly portable across almost any Arduino-compatible microcontroller without relying on specific operating systems like FreeRTOS or high-spec CPUs.

## Key Features

1.  **Hardware Agnostic**: Fully decoupled from physical `HardwareSerial` instantiation. It accepts a pointer to `Stream*`, allowing it to work with `SoftwareSerial`, `Serial1`, `Serial2`, or other standard UART abstractions.
2.  **No Dynamic Allocation**: Say goodbye to heap fragmentation and memory leaks! This library strictly avoids Arduino `String` objects, exclusively using `char` arrays and format buffers for string manipulation.
3.  **Non-Blocking AT Queue**: Reads data iteratively at a per-byte level within the `loop()`, naturally eliminating busy-waits.
4.  **Callback-Driven Architecture**: Easily attach custom callback functions for both real-time status updates and raw UART debugging logs.

## Portability Checklist
✅ AVR (ATmega328, Arduino Uno/Nano, Mega)
✅ ESP8266 & ESP32 (all variants)
✅ STM32 (BluePill, BlackPill)
✅ PIC (via Arduino framework)
✅ Any `Stream` capable framework

---

## Minimal Example (SoftwareSerial on AVR)

```cpp
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "UniversalBT1026.h"

// Define a SoftwareSerial port (RX = 10, TX = 11)
SoftwareSerial btSerial(10, 11);

// Initialize the universal driver instance
UniversalBT1026 bt;

// State Change Callback
void btStateChanged(UniversalBT1026::BTState state) {
    if (state == UniversalBT1026::PLAYING) {
        Serial.println("Audio is now PLAYING.");
    } else if (state == UniversalBT1026::CONNECTED) {
        Serial.println("Device CONNECTED/PAUSED.");
    }
}

// Log Callback (Optional, strictly char* for memory safety)
void btLog(const char* msg) {
    Serial.print("SYS_LOG: ");
    Serial.println(msg);
}

void setup() {
    Serial.begin(115200);
    btSerial.begin(115200); // 115200 baud required by BT1026 default fw

    Serial.println("Starting Universal BT1026 Driver...");

    // Bind log listeners
    bt.onStateChanged(btStateChanged);
    bt.onLog(btLog);

    // Pass the Stream pointer and execute setup sequence
    bt.begin(&btSerial);
}

void loop() {
    // MUST execute continuously in the main loop to process incoming UART data
    bt.loop();
    
    // Simulate user buttons using Serial monitor for testing 
    if (Serial.available()) {
        char c = Serial.read();
        if (c == 'p') bt.play();
        if (c == 's') bt.pause();
        if (c == 'n') bt.next();
        if (c == 'b') bt.prev();
    }
}
```

## API Reference
*   `begin(Stream* serial)` - Binds a software or hardware serial stream.
*   `loop()` - Polls the RX buffer and runs automatic background pings (e.g., `AT+A2DPSTAT`).
*   `sendCommand(const char* cmd)` - Dispatches raw strings to the BT module.
*   `play()`, `pause()`, `next()`, `prev()` - Media transport shortcuts.
*   `onStateChanged(StateCallback cb)` - Register a listener for connection events.
*   `onLog(LogCallback cb)` - Register a listener for raw text output.