#include "servo_task.h"

ServoTask::ServoTask() {}

void ServoTask::begin(Servo *servo, uint8_t minAng, uint8_t maxAng) {
    this->servo = servo;
    minA = minAng;
    maxA = maxAng;
    currentAngle = (uint8_t)(minA + maxA) / 2.0f;
    targetAngle = currentAngle;
    startAngle = currentAngle;
    moving = false;
    if (servo) servo->write((int)currentAngle);
}

void ServoTask::setTarget(uint8_t angle, uint16_t duration) {
    if (!servo) return;
    if (angle < minA) angle = minA;
    if (angle > maxA) angle = maxA;
    if (duration == 0) {
        servo->write(angle);
        currentAngle = angle;
        targetAngle = angle;
        moving = false;
    }
    else {
        startAngle = currentAngle;
        targetAngle = angle;
        moveStart = millis();
        moveDuration = duration;
        moving = true;
    }
}

void ServoTask::moveTo(uint8_t angle) {
    int time = 25;
    static int prev_angle = 90;
        if(angle >= prev_angle) {
        for(int i = prev_angle; i <= angle; i++) {
            servo->write(i); //move to this angle
            delay(time);
            }  
        }
        else{
            for(int i = prev_angle; i >= angle; i--) {
                servo->write(i);
                delay(time);
            }
        }
        prev_angle = angle;
}

void ServoTask::turn_head(float turn_angle) {
    float a = turn_angle;
    float factor;
    if (a <= 60.0f) {
        factor = 1.15f;
    } else if (a >= 120.0f) {
        factor = 0.85f;
    } else if (a <= 90.0f) {
        // 60 → 90 : 1.15 → 1.0
        float t = (a - 60.0f) / 30.0f;
        // smoothstep
        t = t * t * (3.0f - 2.0f * t);
        factor = 1.15f - 0.15f * t;
    }else {
        // 90 → 120 : 1.0 → 0.85
        float t = (a - 90.0f) / 30.0f;
        // smoothstep
        t = t * t * (3.0f - 2.0f * t);
        factor = 1.0f - 0.15f * t;
    }
    float output = a * factor;
    moveTo(output);
}

void ServoTask::update() {
    if (!servo) return;
    uint32_t now = millis();
    if (moving) {
        uint32_t elapsed = now - moveStart;
        if (elapsed >= moveDuration) {
            servo->write(targetAngle);
            currentAngle = targetAngle;
            moving = false;
        }
        else {
            float t = (float)elapsed / (float)moveDuration;
            float ang = startAngle + t * ((float)targetAngle - startAngle);
            currentAngle = ang;
            servo->write((int)ang);
        }
        return;
    }
}
