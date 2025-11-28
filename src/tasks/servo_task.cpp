#include "servo_task.h"

ServoTask::ServoTask() {}

void ServoTask::begin(Servo *srv, uint8_t minAng, uint8_t maxAng) {
    s = srv;
    minA = minAng;
    maxA = maxAng;
    currentAngle = (minA + maxA) / 2.0f;
    targetAngle = (uint8_t)currentAngle;
    startAngle = currentAngle;
    moveStart = 0;
    moveDuration = 0;
    moving = false;
    if (s) s->write((int)currentAngle);
}

void ServoTask::setTarget(uint8_t angle, uint16_t duration) {
    if (!s) return;
    if (angle < minA) angle = minA;
    if (angle > maxA) angle = maxA;
    if (duration == 0) {
        s->write(angle);
        currentAngle = angle;
        targetAngle = angle;
        moving = false;
    } else {
        startAngle = currentAngle;
        targetAngle = angle;
        moveStart = millis();
        moveDuration = duration;
        moving = true;
    }
}

void ServoTask::update() {
    if (!s) return;
    uint32_t now = millis();
    if (moving) {
        uint32_t elapsed = now - moveStart;
        if (elapsed >= moveDuration) {
            // done
            s->write(targetAngle);
            currentAngle = targetAngle;
            moving = false;
        } else {
            float t = (float)elapsed / (float)moveDuration;
            float ang = startAngle + t * ((float)targetAngle - startAngle);
            currentAngle = ang;
            s->write((int)ang);
        }
        return;
    }
}
