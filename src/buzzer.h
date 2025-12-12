#pragma once
#include <Arduino.h>

#define BUZZER 28

void playTone(uint16_t frequency, uint16_t duration);
void playStartupTone();
void playTrackingOnTone();
void playTrackingOffTone();
void playErrorTone();
