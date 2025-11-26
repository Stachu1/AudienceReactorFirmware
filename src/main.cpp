#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include "tasks/fade_task.h"
#include "tasks/icon_task.h"
#include "tasks/servo_task.h"
#include "uart_handler.h"

#define RGB_R 17
#define RGB_G 16
#define RGB_B 25
#define NEO_PWR 11
#define NEO 12
#define NUMPIXELS 1
#define STRIP 7
#define STRIP_LEN 100

#define ICON_COUNT 5

#define SERVO1 3
#define SERVO2 4
#define SERVO3 2


Adafruit_NeoPixel pixel(1, NEO, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_LEN, STRIP, NEO_GRB + NEO_KHZ800);

// Servos
Servo servo1;
Servo servo2;
Servo servo3;

// Tasks (instances)
FadeTask pixelFade;
FadeTask stripFade;
IconTask iconTask;
ServoTask servoTask1;
ServoTask servoTask2;
ServoTask servoTask3;

UartHandler uartHandler;


void setup() {
    Serial.begin(115200);

    pinMode(RGB_R, OUTPUT);
    pinMode(RGB_G, OUTPUT);
    pinMode(RGB_B, OUTPUT);
    pinMode(NEO_PWR, OUTPUT);

    digitalWrite(NEO_PWR, HIGH);
    digitalWrite(RGB_R, HIGH);
    digitalWrite(RGB_G, HIGH);
    digitalWrite(RGB_B, HIGH);

    pixel.begin();
    pixel.setBrightness(128);
    strip.begin();
    strip.setBrightness(55);

    // initialize only the non-icon portion of the strip (icons are first ICON_COUNT LEDs)
    uint16_t nonIconLen = STRIP_LEN - ICON_COUNT;
    strip.fill(strip.Color(255, 0, 0), ICON_COUNT, nonIconLen);
    strip.show();
    delay(200);
    strip.fill(strip.Color(0, 255, 0), ICON_COUNT, nonIconLen);
    strip.show();
    delay(200);
    strip.fill(strip.Color(0, 0, 255), ICON_COUNT, nonIconLen);
    strip.show();
    delay(200);

    // prepare non-blocking fade tasks
    uint8_t c0[3] = {255, 0, 0};
    uint8_t c1[3] = {0, 255, 0};
    uint8_t c2[3] = {0, 0, 255};
    pixelFade.begin(&pixel, c0, c1, c2, 2000); // 2s fades (single pixel)
    // strip fade should not touch icon LEDs (first ICON_COUNT LEDs)
    stripFade.begin(&strip, c0, c1, c2, 3000, ICON_COUNT, nonIconLen); // 3s fades on non-icon region

    // helper: set initial icon colors to off
    for (uint8_t i=0;i<ICON_COUNT;i++) {
        strip.setPixelColor(i, strip.Color(0,0,0));
    }
    strip.show();

    // start icon cycling (one icon lit at a time every 500ms)
    iconTask.begin(&strip, ICON_COUNT, 500, strip.Color(255,255,255));

    // attach servos and start their tasks
    servo1.attach(SERVO1);
    servo2.attach(SERVO2);
    servo3.attach(SERVO3);
    // sweep ranges and periods (different periods/offsets so they move independently)
    servoTask1.begin(&servo1, 10, 170);
    servoTask2.begin(&servo2, 30, 150);
    servoTask3.begin(&servo3, 0, 120);

    // start UART handler and pass iconTask for commands
    uartHandler.begin(&iconTask);
}

void loop() {
    // update leds
    pixelFade.update();
    stripFade.update();

    // handle incoming UART commands
    uartHandler.update();

    // update servos
    servoTask1.update();
    servoTask2.update();
    servoTask3.update();
}