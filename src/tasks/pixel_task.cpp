#include "pixel_task.h"

PixelTask::PixelTask() {}

void PixelTask::begin(Adafruit_NeoPixel *pixel) {
    this->pixel = pixel;
    status = IDLE;
    lastUpdate = 0;
    phase = 0.0f;
    uartBlinkStart = 0;
    if (pixel) {
        pixel->setPixelColor(0, pixel->Color(0, 0, 0));
        pixel->show();
    }
}

void PixelTask::setStatus(PixelStatus status) {
    this->status = status;
}

void PixelTask::triggerUartBlink() {
    status = UART;
    uartBlinkStart = millis();
}

void PixelTask::update() {
    if (!pixel) return;
    uint32_t now = millis();
    
    // handle different status modes
    switch (status) {
        case IDLE: {
            // green breathing effect
            phase += (now - lastUpdate) / 160.0f;
            if (phase > TWO_PI) phase -= TWO_PI;
            float brightness = (sin(phase) + 1.0f) / 2.0f;
            pixel->setPixelColor(0, pixel->Color(0, (uint8_t)(brightness * 255.0f), 0));
            break;
        }
        
        case UART: {
            // solid blue
            uint32_t elapsed = now - uartBlinkStart;
            if (elapsed < UART_BLINK_DURATION) {
                pixel->setPixelColor(0, pixel->Color(0, 0, 255));
            }
            else {
                pixel->setPixelColor(0, pixel->Color(0, 0, 0));
                status = IDLE;
            }
            break;
        }

        case TRACKING: {
            // orange breathing effect
            phase += (now - lastUpdate) / 40.0f;
            if (phase > TWO_PI) phase -= TWO_PI;
            float brightness = (sin(phase) + 1.0f) / 2.0f;
            pixel->setPixelColor(0, pixel->Color((uint8_t)(brightness * 255.0f), (uint8_t)(brightness * 50.0f), 0));
            break;
        }
            
        case ERROR: {
            // solid red
            pixel->setPixelColor(0, pixel->Color(255, 0, 0));
            break;
        }
    }
    pixel->show();
    lastUpdate = now;
}