#pragma once
#include "servo_task.h"
#include <Arduino.h>

class RadarTask {
public:
    RadarTask();
    void begin(HardwareSerial* serial, ServoTask* turn_task, uint32_t baud = 256000);
    void update();
    float getAngle();
    float getDistance();
    
    int16_t x;
    int16_t y;
    bool detected;
    bool tracking;

private:
    HardwareSerial* radarSerial;
    ServoTask* turn_task;
    bool parseData(const uint8_t* buffer, uint32_t len);
    
    enum State {
        WAIT_AA,
        WAIT_FF,
        WAIT_03,
        WAIT_00,
        RECEIVE_FRAME
    };
    
    State state;
    uint8_t buffer[30];
    uint32_t index;
    uint32_t lastUpdate;
    int8_t TURN_OFFSET = 82;
    uint16_t UPDATE_INTERVAL = 25;
};
