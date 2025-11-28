#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_R 17
#define RGB_G 16
#define RGB_B 25

enum PixelStatus {
    IDLE,      // green breathing
    UART,      // blue blink
    RADAR,     // orange blink
    ERROR      // red solid
};

class PixelTask {
public:
    PixelTask();
    void begin(Adafruit_NeoPixel *pixel);
    void update();
    void setStatus(PixelStatus status);
    void triggerUartBlink();

private:
    Adafruit_NeoPixel *pixel = nullptr;
    PixelStatus currentStatus = IDLE;
    uint32_t lastUpdate;
    float phase;
    uint32_t uartBlinkStart;
    static const uint32_t UART_BLINK_DURATION = 200; // ms
};