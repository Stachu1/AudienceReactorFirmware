#pragma once
#include <Arduino.h>

class RadarTask {
public:
    RadarTask();
    void begin(HardwareSerial* serial, uint32_t baud = 256000);
    void update();
    float getAngle();
    float getDistance();
    
    int16_t x;
    int16_t y;
    bool detected;

private:
    HardwareSerial* radarSerial;
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
};
