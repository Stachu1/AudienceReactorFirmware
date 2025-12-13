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

void ServoTask::nodOnce(uint8_t angle, uint16_t moveMs, uint16_t pauseMs) {
    // Starts and restarts a nod
    nodAngle = angle;
    nodMoveMs = moveMs;
    nodPauseMs = pauseMs;

    nodState = NOD_OUT;
    nodT0 = millis();

    setTarget(nodAngle, nodMoveMs); // From origin to nod
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
    // Nodding States tracking
    if (nodState == NOD_IDLE) return;

    // Having this here allows for stage checking and advancing
    if (moving) return;

    switch (nodState) {
        case NOD_OUT:
            nodState = NOD_BACK;
            setTarget(90, nodMoveMs);      // angle -> 90
            break;

        case NOD_BACK:
            nodState = NOD_PAUSE;
            nodT0 = now;                   // start pause timer at 90
            break;

        case NOD_PAUSE:
            if (now - nodT0 >= nodPauseMs) {
                nodState = NOD_IDLE;       // finished one nod
            }
            break;

        default:
            nodState = NOD_IDLE;
            break;
    }
}

