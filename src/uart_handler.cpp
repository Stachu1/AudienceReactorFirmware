#include "uart_handler.h"
#include <string.h>

UartHandler::UartHandler() {}

void UartHandler::begin(IconTask *icons_) {
    icons = icons_;
    idx = 0;
    memset(buf, 0, BUF_SZ);
}

void UartHandler::update() {
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\r') continue;
        if (c == '\n') {
            buf[idx] = '\0';
            if (idx > 0) handleLine(buf);
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
    } else {
        Serial.print("Unknown command: ");
        Serial.println(tmp);
    }
}

void UartHandler::parseIconCommand(char *args) {
    // args: " <index> <color>"
    trim(args);
    if (!icons) return;
    char *tok = strtok(args, " \t");
    if (!tok) return;
    int idx = atoi(tok);
    tok = strtok(NULL, " \t");
    if (!tok) return;
    // color can be a name or r,g,b
    if (strcasecmp(tok, "red") == 0) {
        icons->setIconColor(idx, 255,0,0);
    } else if (strcasecmp(tok, "orange") == 0) {
        icons->setIconColor(idx, 255,100,0);
    } else if (strcasecmp(tok, "yellow") == 0) {
        icons->setIconColor(idx, 255,255,0);
    } else if (strcasecmp(tok, "green") == 0) {
        icons->setIconColor(idx, 0,255,0);
    } else if (strcasecmp(tok, "cyan") == 0) {
        icons->setIconColor(idx, 0,255,255);
    } else if (strcasecmp(tok, "blue") == 0) {
        icons->setIconColor(idx, 0,0,255);
    } else if (strcasecmp(tok, "magenta") == 0) {
        icons->setIconColor(idx, 255,0,255);
    } else if (strcasecmp(tok, "white") == 0) {
        icons->setIconColor(idx, 255,255,255);    
    } else if (strcasecmp(tok, "off") == 0 || strcasecmp(tok, "clear") == 0) {
        icons->clearIcon(idx);
    } else {
        // try r,g,b
        int r=0,g=0,b=0;
        if (sscanf(tok, "%d,%d,%d", &r,&g,&b) == 3) {
            icons->setIconColor(idx, (uint8_t)r, (uint8_t)g, (uint8_t)b);
        } else {
            Serial.print("Unknown color: "); Serial.println(tok);
        }
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
