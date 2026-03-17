#include "UniversalBT1026.h"

UniversalBT1026::UniversalBT1026() : 
    _serial(nullptr), 
    _currentState(UNKNOWN),
    _stateCb(nullptr),
    _logCb(nullptr),
    _rxIndex(0),
    _lastPollMs(0) 
{}

bool UniversalBT1026::begin(Stream* btSerial, int rxPin, int txPin) {
    if (!btSerial) return false;
    _serial = btSerial;
    
    // Сбрасываем модуль AT командой
    sendCommand("AT+RESET");
    
    // Пробуждаемся, настраиваем
    return true;
}

// Заменяет FreeRTOS Task (Superloop дизайн)
void UniversalBT1026::loop() {
    // 1. Чтение ответов порта по байтам, формирование строк
    while (_serial && _serial->available()) {
        char c = (char)_serial->read();
        if (c == '\n' || c == '\r') {
            if (_rxIndex > 0) {
                _rxBuffer[_rxIndex] = '\0';
                _processLine(_rxBuffer);
                _rxIndex = 0;
            }
        } else {
            if (_rxIndex < sizeof(_rxBuffer) - 1) {
                _rxBuffer[_rxIndex++] = c;
            }
        }
    }

    // 2. Периодический опрос AT+A2DPSTAT (раз в 2 сек)
    uint32_t now = millis();
    if (now - _lastPollMs >= 2000) {
        _lastPollMs = now;
        sendCommand("AT+A2DPSTAT"); // Или AT+DEVSTAT
    }
}

void UniversalBT1026::_processLine(const char* line) {
    // Вызов лога, если подписаны
    if (_logCb) {
        char logMsg[128];
        snprintf(logMsg, sizeof(logMsg), "[BT] %s", line);
        _logCb(logMsg);
    }
    
    // Парсим UART ответы (AT+A2DPSTAT=...)
    if (strstr(line, "+A2DPSTAT=PLAY") != nullptr) {
        if (_currentState != PLAYING) {
            _currentState = PLAYING;
            if (_stateCb) _stateCb(_currentState);
        }
    } else if (strstr(line, "+A2DPSTAT=PAUSE") != nullptr) {
        if (_currentState != CONNECTED) { // или DISCONNECTED
            _currentState = CONNECTED;
            if (_stateCb) _stateCb(_currentState);
        }
    }
}

void UniversalBT1026::sendCommand(const char* cmd) {
    if (_serial) {
        // Отправка без очередей xQueue
        _serial->println(cmd);
        if (_logCb) {
             char logMsg[128];
             snprintf(logMsg, sizeof(logMsg), "[BT TX] %s", cmd);
             _logCb(logMsg);
        }
    }
}

// Управление
void UniversalBT1026::play() { sendCommand("AT+PLAY"); }
void UniversalBT1026::pause() { sendCommand("AT+PAUSE"); }
void UniversalBT1026::next() { sendCommand("AT+FORWARD"); }
void UniversalBT1026::prev() { sendCommand("AT+BACKWARD"); }
