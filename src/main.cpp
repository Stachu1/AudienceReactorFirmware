#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include "tasks/icon_task.h"
#include "tasks/servo_task.h"
#include "tasks/pixel_task.h"
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
PixelTask pixelTask;
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
    pixel.setBrightness(100);
    strip.begin();
    strip.setBrightness(55);

    // initialize pixel status task
    pixelTask.begin(&pixel);

    strip.fill(strip.Color(255, 0, 0), 0, STRIP_LEN);
    strip.show();
    delay(200);
    strip.fill(strip.Color(0, 255, 0), 0, STRIP_LEN);
    strip.show();
    delay(200);
    strip.fill(strip.Color(0, 0, 255), 0, STRIP_LEN);
    strip.show();
    delay(200);


    // helper: set initial icon colors to off
    for (uint8_t i=0;i<ICON_COUNT;i++) {
        strip.setPixelColor(i, strip.Color(0,0,0));
    }
    strip.show();

    // start icon cycling (one icon lit at a time every 500ms)
    iconTask.begin(&strip, ICON_COUNT, 500, strip.Color(255,255,255));

    // attach servos and start their tasks
    servo1.attach(SERVO1, 500, 2500);
    servo2.attach(SERVO2, 500, 2500);
    servo3.attach(SERVO3, 500, 2500);
    // sweep ranges and periods (different periods/offsets so they move independently)
    servoTask1.begin(&servo1);
    servoTask2.begin(&servo2);
    servoTask3.begin(&servo3);

    // start UART handler and pass iconTask and servo tasks for commands
    uartHandler.begin(&iconTask, &servoTask1, &servoTask2, &servoTask3, &pixelTask);
}

void loop() {
    // update pixel status indicator
    pixelTask.update();

    // handle incoming UART commands
    uartHandler.update();
    
    // update servo tasks
    servoTask1.update();
    servoTask2.update();
    servoTask3.update();
}