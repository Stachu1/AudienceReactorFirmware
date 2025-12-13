#include "uart_handler.h"

UartHandler::UartHandler() {}

void UartHandler::begin(IconTask *icons, ServoTask *s1, ServoTask *s2, ServoTask *s3, PixelTask *pixel, TimerDisplayTask *display, RadarTask *radar, BodyColorTask *bodyColor) {
    this->icons = icons;
    this->servo1 = s1;
    this->servo2 = s2;
    this->servo3 = s3;
    this->pixel = pixel;
    this->display = display;
    this->radar = radar;
    this->bodyColor = bodyColor;
    idx = 0;
    memset(buf, 0, BUF_SZ);
    tickCounter = 0;
    lastTpsUpdate = millis();
    tps = 0;
}

void UartHandler::update() {
    // Update TPS counter
    tickCounter++;
    uint32_t now = millis();
    if (now - lastTpsUpdate >= 1000) {
        tps = tickCounter;
        tickCounter = 0;
        lastTpsUpdate = now;
    }
    
    while (Serial.available()) {
        uint8_t c = (uint8_t)Serial.read();
        if (c == '\r') continue;
        if (c == '\n') {
            buf[idx] = '\0';
            if (idx > 0) {
                if (pixel) pixel->triggerUartBlink();
                handleLine(buf);
            }
            idx = 0;
            memset(buf, 0, BUF_SZ);
        }
        else {
            if (idx < BUF_SZ-1) {
                buf[idx++] = c;
            }
            else {
                // buffer overflow error
                Serial.println("\033[31mError: UART command buffer overflow\033[0m");
                if (pixel) pixel->setStatus(ERROR);
                return;
            }
        }
    }
}

void UartHandler::handleLine(const char *line) {
    const char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '>' || *p == '$' || *p == '/') p++;
    // make a copy to a modifiable buffer
    char tmp[BUF_SZ];
    strncpy(tmp, p, BUF_SZ-1);
    tmp[BUF_SZ-1] = '\0';
    trim(tmp);
    if (strncmp(tmp, "icon", 4) == 0) {
        parseIconCommand(tmp+4);
    }
    else if (strncmp(tmp, "servo", 5) == 0) {
        parseServoCommand(tmp+5);
    }
    else if (strncmp(tmp, "timer", 5) == 0) {
        parseTimerCommand(tmp+5);
    }
    else if (strncmp(tmp, "tracking", 8) == 0) {
        parseTrackingCommand(tmp+8);
    }
    else if (strncmp(tmp, "body", 4) == 0) {
        parseBodyCommand(tmp+4);
    }
    else if (strncmp(tmp, "tps", 3) == 0) {
        parseTpsCommand(tmp+3);
    }
    else if (strncmp(tmp, "nod", 3) == 0) {
        parseNodCommand(tmp+3);
    }
    else {
        Serial.print("Unknown command: ");
        Serial.println(tmp);
    }
}

void UartHandler::parseIconCommand(char *args) {
    // args: " <index>
    trim(args);
    if (!icons) return;
    char *tok = strtok(args, " \t");
    if (!tok) return;
    int idx = atoi(tok);
    tok = strtok(NULL, " \t");
    if (!tok) return;
    icons->setIcon(args);
}

void UartHandler::parseServoCommand(char *args) {
    // args: <id> <angle> [<duration_ms>|none]
    trim(args);
    char *tok = strtok(args, " \t");
    if (!tok) return;
    int id = atoi(tok);
    tok = strtok(NULL, " \t");
    if (!tok) return;
    int angle = atoi(tok);
    tok = strtok(NULL, " \t");

    uint16_t dur = 0;
    if (tok) {
        dur = (int16_t)atol(tok);
    }
    ServoTask *st = nullptr;
    if (id == 1) st = servo1;
    else if (id == 2) st = servo2;
    else if (id == 3) st = servo3;
    if (!st) {
        Serial.print("\033[31mError: Servo ");
        Serial.print(id);
        Serial.println(" not configured");
        if (pixel) pixel->setStatus(ERROR);
        return;
    }
    st->setTarget((int16_t)angle, dur);
    Serial.print("Servo "); Serial.print(id); Serial.print(" -> "); Serial.print(angle);
    Serial.print(" over "); Serial.print(dur); Serial.println(" ms");
}

void UartHandler::parseTimerCommand(char *args) {
    // args: <seconds>|start <seconds>|stop
    trim(args);
    if (strncmp(args, "start", 5) == 0) {
        // start timer
        if (display) {
            int seconds = atoi(args);
            extern TimerDisplayTask displayTask;
            displayTask.start((uint32_t)seconds*1000UL);
            Serial.println("Timer started");
        }
    }
    else if (strncmp(args, "stop", 4) == 0) {
        // stop timer
        if (display) {
            display->stop();
            Serial.println("Timer stopped");
        }
    }
    else {
        // set time
        int seconds = atoi(args);
        extern TimerDisplayTask displayTask;
        displayTask.show((uint32_t)seconds*1000UL);
        Serial.print("Timer set to ");
        Serial.print(seconds);
        Serial.println(" seconds");
    }
}

void UartHandler::parseTrackingCommand(char *args) {
    // args: on|off
    trim(args);
    if (!radar) {
        Serial.println("\033[31mError: Radar not configured\033[0m");
        if (pixel) pixel->setStatus(ERROR);
        return;
    }

    if (strcasecmp(args, "on") == 0) {
        radar->tracking = true;
        pixel->setStatus(TRACKING);
        playTrackingOnTone();
        Serial.println("Radar tracking enabled");
    }
    else if (strcasecmp(args, "off") == 0) {
        radar->tracking = false;
         pixel->setStatus(IDLE);
         playTrackingOffTone();
        Serial.println("Radar tracking disabled");
    }
    else {
        Serial.print("\033[31mError: Invalid tracking command: ");
        Serial.println(args);
        Serial.println("Use 'tracking on' or 'tracking off'\033[0m");
        if (pixel) pixel->setStatus(ERROR);
    }
}

void UartHandler::parseBodyCommand(char *args) {
    // args: <color>
    trim(args);
    if (!bodyColor) {
        Serial.println("\033[31mError: Body color task not configured\033[0m");
        if (pixel) pixel->setStatus(ERROR);
        return;
    }
    if (strcmp(args, "average") != 0 && strcmp(args, "good") != 0 && strcmp(args, "extreme") != 0) {
        Serial.print("\033[31mError: Invalid body color command: ");
        Serial.println(args);
        Serial.println("Use 'body average', 'body good' or 'body extreme'\033[0m");
        if (pixel) pixel->setStatus(ERROR);
        return;
    }
    bodyColor->setColor(String(args));

}

void UartHandler::parseTpsCommand(char *args) {
    Serial.print(tps);
    Serial.println(" ticks/sec");
}

void UartHandler::parseNodCommand(char *args) {
    if (!servo2 || !servo3) {
        Serial.println("\033[31mError: Servo tasks not configured\033[0m");
        if (pixel) pixel->setStatus(ERROR);
        return;
    }
    
    servo2->nod(20, 500, 0);
    servo3->nod(160, 500, 0);
    Serial.println("Nod triggered");
}

void UartHandler::trim(char *s) {
    // trim leading
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p)+1);
    // trim trailing
    int len = strlen(s);
    while (len>0 && isspace((unsigned char)s[len-1])) s[--len] = '\0';
}
