#include "uart_handler.h"
#include <string.h>

UartHandler::UartHandler() {}

void UartHandler::begin(IconTask *icons, ServoTask *s1, ServoTask *s2, ServoTask *s3, PixelTask *pix) {
    this->icons = icons;
    this->servo1 = s1;
    this->servo2 = s2;
    this->servo3 = s3;
    this->pixel = pix;
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
            memset(buf,0,BUF_SZ);
        } else {
            if (idx < BUF_SZ-1) buf[idx++] = c;
        }
    }
}

void UartHandler::handleLine(const char *line) {
    // Accept lines starting with '>' or not
    const char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '>') p++;
    // make a copy to a modifiable buffer
    char tmp[BUF_SZ];
    strncpy(tmp, p, BUF_SZ-1);
    tmp[BUF_SZ-1] = '\0';
    trim(tmp);
    if (strncmp(tmp, "icon", 4) == 0) {
        parseIconCommand(tmp+4);
    } else if (strncmp(tmp, "servo", 5) == 0) {
        // delegate to parseServoCommand
        char args[BUF_SZ];
        strncpy(args, tmp+5, BUF_SZ-1);
        args[BUF_SZ-1] = '\0';
        trim(args);
        parseServoCommand(args);
    } else {
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
    bool validColor = false;
    
    if (strcasecmp(tok, "red") == 0) {
        r=255; g=0; b=0; validColor=true;
    } else if (strcasecmp(tok, "orange") == 0) {
        r=255; g=100; b=0; validColor=true;
    } else if (strcasecmp(tok, "yellow") == 0) {
        r=255; g=255; b=0; validColor=true;
    } else if (strcasecmp(tok, "green") == 0) {
        r=0; g=255; b=0; validColor=true;
    } else if (strcasecmp(tok, "cyan") == 0) {
        r=0; g=255; b=255; validColor=true;
    } else if (strcasecmp(tok, "blue") == 0) {
        r=0; g=0; b=255; validColor=true;
    } else if (strcasecmp(tok, "magenta") == 0) {
        r=255; g=0; b=255; validColor=true;
    } else if (strcasecmp(tok, "white") == 0) {
        r=255; g=255; b=255; validColor=true;    
    } else if (strcasecmp(tok, "off") == 0 || strcasecmp(tok, "clear") == 0) {
        icons->clearIcon(idx);
        return;
    } else {
        // try r,g,b
        if (sscanf(tok, "%d,%d,%d", (int*)&r, (int*)&g, (int*)&b) == 3) {
            validColor = true;
        } else {
            Serial.print("Unknown color: "); Serial.println(tok);
            return;
        }
    }
    
    if (validColor) {
        if (blinkInterval > 0) {
            icons->setIconBlink(idx, r, g, b, blinkInterval);
            Serial.print("Icon "); Serial.print(idx); Serial.print(" blinking at "); Serial.print(blinkInterval); Serial.println(" ms");
        } else {
            icons->setIconColor(idx, r, g, b);
        }
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
    // If the user passes a duration (ms) use it. If they pass 'none' or omit it,
    // interpret as "as fast as possible" — use a short default duration instead
    uint16_t dur = 0;
    if (!tok) {
        dur = 0;
    } else if (strcasecmp(tok, "none") == 0 || strcasecmp(tok, "fast") == 0) {
        dur = 0;
    } else {
        dur = (int16_t)atol(tok);
    }
    ServoTask *st = nullptr;
    if (id == 1) st = servo1;
    else if (id == 2) st = servo2;
    else if (id == 3) st = servo3;
    if (!st) {
        Serial.print("Unknown servo id: "); Serial.println(id);
        return;
    }
    st->setTarget((int16_t)angle, dur);
    Serial.print("Servo "); Serial.print(id); Serial.print(" -> "); Serial.print(angle);
    Serial.print(" over "); Serial.print(dur); Serial.println(" ms");
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
