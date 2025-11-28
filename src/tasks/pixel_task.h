#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum PixelStatus {
    IDLE,      // green breathing
    UART,      // blue blink
    ERROR      // red solid
};

class PixelTask {
public:
    PixelTask();
    void begin(Adafruit_NeoPixel *t);
    void update();
    void setStatus(PixelStatus status);
    void triggerUartBlink(); // trigger a single blue blink sequence

private:
    Adafruit_NeoPixel *target = nullptr;
    PixelStatus currentStatus = IDLE;
    uint32_t lastUpdate = 0;
    
    // idle breathing state
    float breathPhase = 0.0f;
    
    // uart blink state
    bool uartBlinking = false;
    uint32_t uartBlinkStart = 0;
    static const uint32_t UART_BLINK_DURATION = 200; // ms
};