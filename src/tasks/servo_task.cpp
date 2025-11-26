#include "servo_task.h"

ServoTask::ServoTask() {}

void ServoTask::begin(Servo *srv, int16_t minAng, int16_t maxAng) {
    s = srv;
    minA = minAng;
    maxA = maxAng;
    currentAngle = (minA + maxA) / 2.0f;
    targetAngle = (int16_t)currentAngle;
    startAngle = currentAngle;
    moveStartMs = 0;
    moveDurationMs = 0;
    moving = false;
    if (s) s->write((int)currentAngle);
}

void ServoTask::setTarget(int16_t angle, uint16_t durationMs) {
    if (!s) return;
    if (angle < minA) angle = minA;
    if (angle > maxA) angle = maxA;
    if (durationMs == 0) {
        s->write(angle);
        currentAngle = angle;
        targetAngle = angle;
        moving = false;
    } else {
        startAngle = currentAngle;
        targetAngle = angle;
        moveStartMs = millis();
        moveDurationMs = durationMs;
        moving = true;
    }
}

void ServoTask::update() {
    if (!s) return;
    unsigned long now = millis();
    if (moving) {
        unsigned long elapsed = now - moveStartMs;
        if (elapsed >= moveDurationMs) {
            // done
            s->write(targetAngle);
            currentAngle = targetAngle;
            moving = false;
        } else {
            float t = (float)elapsed / (float)moveDurationMs;
            float ang = startAngle + t * ((float)targetAngle - startAngle);
            currentAngle = ang;
            s->write((int)ang);
        }
        return;
    }
}
