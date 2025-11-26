#pragma once
#include <Arduino.h>
#include "tasks/icon_task.h"
#include "tasks/servo_task.h"

class UartHandler {
public:
    UartHandler();
    // pass optional servo task pointers (can be nullptr if not used)
    void begin(IconTask *icons, ServoTask *s1=nullptr, ServoTask *s2=nullptr, ServoTask *s3=nullptr);
    void update();
private:
    IconTask *icons = nullptr;
    ServoTask *servo1 = nullptr;
    ServoTask *servo2 = nullptr;
    ServoTask *servo3 = nullptr;
    static const int BUF_SZ = 128;
    char buf[BUF_SZ];
    int idx = 0;
    void handleLine(const char *line);
    void parseIconCommand(char *args);
    void parseServoCommand(char *args);
    void trim(char *s);
};
