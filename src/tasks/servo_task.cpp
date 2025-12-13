#include "servo_task.h"

ServoTask::ServoTask() {}

void ServoTask::begin(Servo *servo, uint8_t minAng, uint8_t maxAng) {
    this->servo = servo;
    minA = minAng;
    maxA = maxAng;
    currentAngle = (uint8_t)(minA + maxA) / 2.0f;
    targetAngle = currentAngle;
    startAngle = currentAngle;
    lastUpdate = 0;
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

void ServoTask::nodding(uint8_t angle) {
    uint32_t now = millis();
    uint16_t half = UPDATE_INTERVAL / 2;

    // only change target once per half interval
    if (now - nodLast < half) return;
    nodLast = now;

    if (nodOut) {
        setTarget(angle, half);
    } else {
        setTarget(100, half);
    }
    nodOut = !nodOut;
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

