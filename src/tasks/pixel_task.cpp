#include "pixel_task.h"

PixelTask::PixelTask() {}

void PixelTask::begin(Adafruit_NeoPixel *pixel) {
    this->pixel = pixel;
    currentStatus = IDLE;
    lastUpdate = 0;
    phase = 0.0f;
    uartBlinkStart = 0;
    if (pixel) {
        pixel->setPixelColor(0, pixel->Color(0, 0, 0));
        pixel->show();
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
    if (!pixel) return;
    uint32_t now = millis();
    
    // handle different status modes
    switch (currentStatus) {
        case IDLE: {
            // green breathing effect (slow fade in/out)
            phase += (now - lastUpdate) / 160.0f;
            if (phase > TWO_PI) phase -= TWO_PI;
            float brightness = (sin(phase) + 1.0f) / 2.0f;
            pixel->setPixelColor(0, pixel->Color(0, (uint8_t)(brightness * 255.0f), 0));
            pixel->show();
            break;
        }
        
        case UART: {
            // solid blue
            uint32_t elapsed = now - uartBlinkStart;
            if (elapsed < UART_BLINK_DURATION) {
                // blue blink on
                pixel->setPixelColor(0, pixel->Color(0, 0, 255));
                pixel->show();
                digitalWrite(RGB_B, LOW);
            }
            else {
                // clear and return to IDLE
                pixel->setPixelColor(0, pixel->Color(0, 0, 0));
                pixel->show();
                currentStatus = IDLE;
                digitalWrite(RGB_B, HIGH);
            }
            break;
        }
            
        case ERROR: {
            // solid red
            pixel->setPixelColor(0, pixel->Color(255, 0, 0));
            pixel->show();
            break;
        }
    }
    
    lastUpdate = now;
}