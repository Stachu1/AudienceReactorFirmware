#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <Servo.h>
#include <TM1637Display.h>
#include "tasks/icon_task.h"
#include "tasks/servo_task.h"
#include "tasks/pixel_task.h"
#include "tasks/display_task.h"
#include "tasks/radar_task.h"
#include "tasks/body_color_task.h"
#include "uart_handler.h"
#include "buzzer.h"

#define RGB_R 17
#define RGB_G 16
#define RGB_B 25
#define NEO_PWR 11
#define NEO 12
#define NUMPIXELS 1

#define STRIP_PIN 7
#define NUM_LEDS 202
#define ICON_COUNT 10

#define SERVO1 3
#define SERVO2 4
#define SERVO3 2

#define TM1637_CLK 27
#define TM1637_DIO 26


Adafruit_NeoPixel pixel(1, NEO, NEO_GRB + NEO_KHZ800);

//LED Strip
CRGB leds[NUM_LEDS];

// TM1637 Display
TM1637Display display(TM1637_CLK, TM1637_DIO);

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
TimerDisplayTask displayTask;
RadarTask radarTask;
BodyColorTask bodyColorTask;

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
    pixel.setBrightness(50);


    // initialize pixel status task
    pixelTask.begin(&pixel);
    displayTask.begin(&display);

    //initialize LED strip
    FastLED.addLeds<WS2812, STRIP_PIN,GRB>(leds,NUM_LEDS);
    FastLED.setBrightness(100);
    iconTask.begin(leds, ICON_COUNT);
    bodyColorTask.begin(leds,NUM_LEDS, ICON_COUNT);
    displayTask.begin(&display);
    bodyColorTask.setColor("idle");
    Serial.print("BodyColor set idle");

    // attach servos and start their tasks
    servo1.attach(SERVO1, 500+222, 2500-444);
    servo2.attach(SERVO2, 500, 2500);
    servo3.attach(SERVO3, 500, 2500);

    //defining servo 1 2 and 3
    servoTask1.begin(&servo1); //rotation of head
    servoTask2.begin(&servo2); //nodding tilting 1
    servoTask3.begin(&servo3); //nodding tilting 2

    // initialize radar task
    radarTask.begin(&Serial1, &servoTask1, 256000);

    // start UART handler and pass iconTask and servo tasks for commands
    uartHandler.begin(&iconTask, &servoTask1, &servoTask2, &servoTask3, &pixelTask, &displayTask, &radarTask, &bodyColorTask);

    // initialize buzzer and play startup tone
    pinMode(BUZZER, OUTPUT);
    playStartupTone();
}

void loop() {
    // handle incoming UART commands
    uartHandler.update();

    // update pixel status indicator
    pixelTask.update();

    // update servo tasks
    servoTask1.update();
    servoTask2.update();
    servoTask3.update();

    // update display task
    displayTask.update();

    // update radar task
    radarTask.update();
}