#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <TM1637Display.h>
#include "tasks/icon_task.h"
#include "tasks/servo_task.h"
#include "tasks/pixel_task.h"
#include "tasks/display_task.h"
#include "tasks/radar_task.h"
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

#define BUZZER 28

#define TM1637_CLK 27
#define TM1637_DIO 26


Adafruit_NeoPixel pixel(1, NEO, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_LEN, STRIP, NEO_GRB + NEO_KHZ800);

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
DisplayTask displayTask;
RadarTask radarTask;

UartHandler uartHandler;

void playTone(uint16_t frequency, uint16_t duration) {
    if (frequency == 0) {
        delay(duration);
        return;
    }
    uint16_t halfPeriod = 1000000U / frequency / 2;
    uint16_t cycles = (uint16_t)frequency * duration / 1000;
    for (uint16_t i = 0; i < cycles; i++) {
        digitalWrite(BUZZER, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(BUZZER, LOW);
        delayMicroseconds(halfPeriod);
    }
}

void playStartupTone() {
    playTone(587, 60);
    playTone(880, 60);
    playTone(0, 10);
    playTone(1175, 110);
}

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
    strip.begin();
    strip.setBrightness(55);

    // initialize pixel status task
    pixelTask.begin(&pixel);
    iconTask.begin(&strip, ICON_COUNT);
    displayTask.begin(&display);

    // attach servos and start their tasks //angles are in PWM pulsewidth in micro sencond
    servo1.attach(SERVO1, 500, 2500);
    servo2.attach(SERVO2, 500, 2500);
    servo3.attach(SERVO3, 500, 2500);

    //defining servo 1 2 and 3
    servoTask1.begin(&servo1); //rotation of head
    servoTask2.begin(&servo2); //nodding tilting 1
    servoTask3.begin(&servo3); //nodding tilting 2

    // initialize radar task // serial and baudrate
    radarTask.begin(&Serial1, 256000);

    // start UART handler and pass iconTask and servo tasks for commands
    uartHandler.begin(&iconTask, &servoTask1, &servoTask2, &servoTask3, &pixelTask, &displayTask);

    // initialize buzzer and play startup tone
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);
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

    // update radar task //updating radar values.
    radarTask.update();

    //get radar angle and send to servo.
    int turn_angle = -radarTask.getAngle()+90;
    servoTask1.moveTo(turn_angle);
    servoTask2.moveTo(turn_angle);
    
    static int count = count++; //this should be exhanged for when the command for nodding gets sendt from pi
    if(count == 1000) {
        servoTask2.nodding();
        servoTask3.nodding();
        count = 0;
    }
    
   
}