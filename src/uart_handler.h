#pragma once
#include <Arduino.h>
#include <string.h>
#include "tasks/icon_task.h"
#include "tasks/servo_task.h"
#include "tasks/pixel_task.h"
#include "tasks/display_task.h"
#include "tasks/radar_task.h"

class UartHandler {
public:
    UartHandler();
    void begin(IconTask *icons, ServoTask *s1=nullptr, ServoTask *s2=nullptr, ServoTask *s3=nullptr, PixelTask *pix=nullptr, DisplayTask *display=nullptr, RadarTask *radar=nullptr);
    void update();
private:
    IconTask *icons = nullptr;
    ServoTask *servo1 = nullptr;
    ServoTask *servo2 = nullptr;
    ServoTask *servo3 = nullptr;
    PixelTask *pixel = nullptr;
    DisplayTask *display = nullptr;
    RadarTask *radar = nullptr;
    static const int BUF_SZ = 128;
    char buf[BUF_SZ];
    int idx = 0;
    void handleLine(const char *line);
    void parseIconCommand(char *args);
    void parseServoCommand(char *args);
    void parseTimerCommand(char *args);
    void parseTrackingCommand(char *args);
    void trim(char *s);
};
