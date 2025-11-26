#pragma once
#include <Arduino.h>
#include "tasks/icon_task.h"

class UartHandler {
public:
    UartHandler();
    void begin(IconTask *icons);
    void update();
private:
    IconTask *icons = nullptr;
    static const int BUF_SZ = 128;
    char buf[BUF_SZ];
    int idx = 0;
    void handleLine(const char *line);
    void parseIconCommand(char *args);
    void trim(char *s);
};
