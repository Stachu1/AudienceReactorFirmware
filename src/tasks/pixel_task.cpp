#include "pixel_task.h"

PixelTask::PixelTask() {}

void PixelTask::begin(Adafruit_NeoPixel *t) {
    target = t;
    currentStatus = IDLE;
    lastUpdate = millis();
    breathPhase = 0.0f;
    uartBlinkStart = 0;
    if (target) {
        target->setPixelColor(0, target->Color(0, 0, 0));
        target->show();
    }
}

void PixelTask::setStatus(PixelStatus status) {
    currentStatus = status;
}

void PixelTask::triggerUartBlink() {
    currentStatus = UART;
    uartBlinkStart = millis();
}

void PixelTask::update() {
    if (!target) return;
    uint32_t now = millis();
    
    // handle different status modes
    switch (currentStatus) {
        case IDLE: {
            // green breathing effect (slow fade in/out)
            breathPhase += 0.002f; // adjust speed here
            if (breathPhase > 2.0f * PI) breathPhase -= 2.0f * PI;
            
            float brightness = (sin(breathPhase) + 1.0f) / 2.0f; // 0..1
            uint8_t green = (uint8_t)(brightness * 255.0f);
            target->setPixelColor(0, target->Color(0, green, 0));
            target->show();
            break;
        }
        
        case UART: {
            // solid blue
            uint32_t elapsed = now - uartBlinkStart;
            if (elapsed < UART_BLINK_DURATION) {
                // blue blink on
                target->setPixelColor(0, target->Color(0, 0, 255));
                target->show();
            } else {
                // clear and return to normal status
                target->setPixelColor(0, target->Color(0, 0, 0));
                target->show();
                currentStatus = IDLE;
            }
            break;
        }
            
        case ERROR: {
            // solid red
            target->setPixelColor(0, target->Color(255, 0, 0));
            target->show();
            break;
        }
    }
    
    lastUpdate = now;
}