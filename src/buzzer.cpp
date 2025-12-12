#include "buzzer.h"

void playTone(uint16_t frequency, uint16_t duration) {
    if (frequency == 0) {
        delay(duration);
        return;
    }
    uint16_t halfPeriod = 1000000U / frequency / 2;
    uint16_t cycles = (uint16_t)frequency * duration / 1000;
    for (uint16_t i = 0; i < cycles; i++) {
        digitalWrite(BUZZER, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(BUZZER, LOW);
        delayMicroseconds(halfPeriod);
    }
}

void playStartupTone() {
    playTone(587, 60);
    playTone(880, 60);
    playTone(0, 10);
    playTone(1175, 110);
}

void playTrackingOnTone() {
    playTone(800, 80);
    playTone(1000, 80);
    playTone(1200, 80);
}

void playTrackingOffTone() {
    playTone(1200, 80);
    playTone(1000, 80);
    playTone(800, 80);
}

void playErrorTone() {
    playTone(500, 100);
    playTone(0, 50);
    playTone(500, 100);
}
