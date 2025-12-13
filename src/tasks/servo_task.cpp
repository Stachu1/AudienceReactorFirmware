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

void ServoTask::nod(uint8_t angle, uint16_t move_ms) {
    nodMoveTime = move_ms;
    nodState = NodState::OUT;
    setTarget(angle, nodMoveTime);
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

    if (nodState == NodState::IDLE || moving) return;

    switch (nodState) {
        case NodState::OUT:
            nodState = NodState::BACK;
            setTarget(90, nodMoveTime);
            break;

        case NodState::BACK:
            nodState = NodState::IDLE;
            break;
    }
}

