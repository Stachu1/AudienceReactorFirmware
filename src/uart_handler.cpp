#include "uart_handler.h"

UartHandler::UartHandler() {}

void UartHandler::begin(IconTask *icons, ServoTask *s1, ServoTask *s2, ServoTask *s3, PixelTask *pix, DisplayTask *display) {
    this->icons = icons;
    this->servo1 = s1;
    this->servo2 = s2;
    this->servo3 = s3;
    this->pixel = pix;
    this->display = display;
    idx = 0;
    memset(buf, 0, BUF_SZ);
}

void UartHandler::update() {
    while (Serial.available()) {
        char c = (char)Serial.read();
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
    else {
        Serial.print("Unknown command: ");
        Serial.println(tmp);
    }
}

void UartHandler::parseIconCommand(char *args) {
    // args: " <index> <color> [<blink_interval>]"
    trim(args);
    if (!icons) return;
    char *tok = strtok(args, " \t");
    if (!tok) return;
    int idx = atoi(tok);
    tok = strtok(NULL, " \t");
    if (!tok) return;
    
    // get optional blink interval
    char *blinkTok = strtok(NULL, " \t");
    uint16_t blinkInterval = 0;
    if (blinkTok) {
        blinkInterval = (uint16_t)atoi(blinkTok);
    }

    // parse color
    uint8_t r=0, g=0, b=0;
    
    if (strcasecmp(tok, "red") == 0) {
        r=255; g=0; b=0;
    }
    else if (strcasecmp(tok, "orange") == 0) {
        r=255; g=100; b=0;
    }
    else if (strcasecmp(tok, "yellow") == 0) {
        r=255; g=255; b=0;
    }
    else if (strcasecmp(tok, "green") == 0) {
        r=0; g=255; b=0;
    }
    else if (strcasecmp(tok, "cyan") == 0) {
        r=0; g=255; b=255;
    }
    else if (strcasecmp(tok, "blue") == 0) {
        r=0; g=0; b=255;
    }
    else if (strcasecmp(tok, "magenta") == 0) {
        r=255; g=0; b=255;
    }
    else if (strcasecmp(tok, "white") == 0) {
        r=255; g=255; b=255;
    }
    else if (strcasecmp(tok, "off") == 0 || strcasecmp(tok, "clear") == 0) {
        icons->clearIcon(idx);
        return;
    }
    else {
        // try r,g,b
        if (sscanf(tok, "%d,%d,%d", (int*)&r, (int*)&g, (int*)&b) != 3) {
            Serial.print("\033[31mError: Unknown color ");
            Serial.println(tok);
            if (pixel) pixel->setStatus(ERROR);
            return;
        }
    }

    if (blinkInterval > 0) {
        icons->setIcon(idx, r, g, b, blinkInterval);
    } else {
        icons->setIcon(idx, r, g, b);
    }
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
    // args: <seconds>|start|stop|brightness <0-7>
    trim(args);
    if (strncmp(args, "start", 5) == 0) {
        // start timer
        if (display) {
            display->start();
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
    else if (strncmp(args, "brightness", 10) == 0) {
        char *tok = strtok(args + 10, " \t");
        if (!tok) return;
        int b = atoi(tok);
        if (b < 0) b = 0;
        if (b > 7) b = 7;
        extern DisplayTask displayTask;
        displayTask.setBrightness((uint8_t)b);
        Serial.print("Display brightness set to ");
        Serial.println(b);
    }
    else {
        // set time
        int seconds = atoi(args);
        extern DisplayTask displayTask;
        displayTask.setTime((uint32_t)seconds);
        Serial.print("Timer set to ");
        Serial.print(seconds);
        Serial.println(" seconds");
    }
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
