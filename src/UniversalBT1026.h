#pragma once

#include <Arduino.h>

class UniversalBT1026 {
public:
    enum BTState { DISCONNECTED, CONNECTED, PLAYING, UNKNOWN };

    // Коллбеки для привязки к Main (начисто, без FreeRTOS)
    typedef void (*StateCallback)(BTState state);
    typedef void (*LogCallback)(const char* msg);

    UniversalBT1026();
    
    // Передаем Serial порт по ссылке (HardwareSerial (AVR) или Stream)
    // Это позволяет использовать Serial1, Serial2 или SoftwareSerial
    bool begin(Stream* btSerial, int rxPin = -1, int txPin = -1);

    // Замена задачи: нужно вызывать в главном loop!
    void loop();

    void sendCommand(const char* cmd);
    
    void onStateChanged(StateCallback cb) { _stateCb = cb; }
    void onLog(LogCallback cb) { _logCb = cb; }

    // Управление
    void play();
    void pause();
    void next();
    void prev();

private:
    Stream* _serial;
    BTState _currentState;

    StateCallback _stateCb;
    LogCallback _logCb;

    char _rxBuffer[128];
    uint8_t _rxIndex;
    
    uint32_t _lastPollMs;

    void _processLine(const char* line);
};