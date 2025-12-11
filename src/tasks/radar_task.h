#pragma once
#include "servo_task.h"
#include <Arduino.h>

class RadarTask {
public:
    RadarTask();
    void begin(HardwareSerial* serial, ServoTask* servo_task, uint32_t baud = 256000);
    void update();
    float getAngle();
    float getDistance();
    float angleCompensation();
    
    int16_t x;
    int16_t y;
    bool detected;
    bool tracking;

private:
    HardwareSerial* radarSerial;
    ServoTask* servo_task;
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
    const int8_t TURN_OFFSET = 85;        // degrees
    const uint16_t UPDATE_INTERVAL = 30;  // milliseconds
    const float X_OFFSET = 29.0f;         // mm
    const float Y_OFFSET = 110.0f;        // mm
};
