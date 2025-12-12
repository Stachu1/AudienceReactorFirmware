#include "radar_task.h"
#include <math.h>

RadarTask::RadarTask() 
    : radarSerial(nullptr), x(0), y(0), detected(false),
      state(WAIT_AA), index(0) {}

void RadarTask::begin(HardwareSerial* serial, ServoTask* turn_task, uint32_t baud) {
    radarSerial = serial; //setting the comunication to serial
    this->turn_task = turn_task;
    radarSerial->begin(baud); //setting baud rate?
    x = 0;
    y = 0;
    detected = false;
    tracking = false;
    state = WAIT_AA; //wait for start command from radar?
    index = 0;
    lastUpdate = 0;
}

void RadarTask::update() {
    if (radarSerial == nullptr) {
        return;
    }

    while (radarSerial->available()) {
        byte byteIn = radarSerial->read();

        switch(state) {
            case WAIT_AA:
                if (byteIn == 0xAA) state = WAIT_FF;
                break;

            case WAIT_FF:
                if (byteIn == 0xFF) state = WAIT_03;
                else state = WAIT_AA;
                break;

            case WAIT_03:
                if (byteIn == 0x03) state = WAIT_00;
                else state = WAIT_AA;
                break;

            case WAIT_00:
                if (byteIn == 0x00) {
                    index = 0;
                    state = RECEIVE_FRAME;
                }
                else {
                    state = WAIT_AA;
                }
                break;

            case RECEIVE_FRAME:
                buffer[index++] = byteIn;
                if (index >= 26) { // 24 bytes data + 2 tail bytes
                    if (buffer[24] == 0x55 && buffer[25] == 0xCC) {
                        parseData(buffer, 24);
                    }
                    state = WAIT_AA;
                    index = 0;
                }
                break;
        }
    }
    uint32_t millistNow = millis();
    if (millistNow - lastUpdate > UPDATE_INTERVAL)
    {
        lastUpdate = millistNow;
        uint8_t turn_angle = -getAngle() + TURN_OFFSET;
        //uint8_t true_angle = angleCompensation(turn_angle);
        turn_task->setTarget(turn_angle, UPDATE_INTERVAL);
    }
    uint32_t millistNow = millis();
    if (millistNow - lastUpdate > UPDATE_INTERVAL)
    {
        lastUpdate = millistNow;
        uint8_t turn_angle = -getAngle() + TURN_OFFSET;
        //uint8_t true_angle = angleCompensation(turn_angle);
        turn_task->setTarget(turn_angle, UPDATE_INTERVAL);
    }
}

bool RadarTask::parseData(const uint8_t* buf, uint32_t len) {
    if (len != 24) {
        return false;
    }

    // Only parse first 8 bytes for the first target
    int16_t raw_x = buf[0] | (buf[1] << 8);
    int16_t raw_y = buf[2] | (buf[3] << 8);
    int16_t raw_speed = buf[4] | (buf[5] << 8);
    uint16_t raw_pixel_dist = buf[6] | (buf[7] << 8);

    detected = !(raw_x == 0 && raw_y == 0 && raw_speed == 0 && raw_pixel_dist == 0);

    // Correctly parse signed values
    x = ((raw_x & 0x8000) ? 1 : -1) * (raw_x & 0x7FFF);
    y = ((raw_y & 0x8000) ? 1 : -1) * (raw_y & 0x7FFF);
    return true;
}

float RadarTask::getAngle() {
    if (detected) {
        // Angle calculation (convert radians to degrees, then flip)
        float angleRad = atan2(y, x) - (PI / 2);
        float angleDeg = angleRad * (180.0 / PI);
        return -angleDeg; // align angle with x measurement positive / negative sign
    } else {
        return 0.0;
    }
}

float RadarTask::getDistance() {
    if (detected) {
        return sqrt((float)(x * x + y * y));
    } else {
        return 0.0;
    }
}