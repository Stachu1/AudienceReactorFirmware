#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

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

struct ServoTask {
    Servo *s = nullptr;
    uint8_t minA = 0;
    uint8_t maxA = 180;
    unsigned long period = 2000; // ms for full back-and-forth cycle

    void begin(Servo *srv, uint8_t minAng, uint8_t maxAng) {
        s = srv;
        minA = minAng;
        maxA = maxAng;
    }

    void update() {
        if (!s) return;
        unsigned long now = millis();
        unsigned long t = now % period;
        float phase = (float)t / (float)period; // 0..1
        float tri;
        if (phase < 0.5f) tri = phase * 2.0f; else tri = (1.0f - phase) * 2.0f; // 0..1 triangular
        int angle = (int)(minA + tri * (float)(maxA - minA));
        s->write(angle);
    }
};

ServoTask servoTask1;
ServoTask servoTask2;
ServoTask servoTask3;



struct FadeTask {
    Adafruit_NeoPixel *target;
    uint8_t colors[3][3]; // 3-step cycle: each color is {r,g,b}
    uint8_t curIdx = 0;   // index of the "from" color
    unsigned long startMs = 0;
    unsigned long durationMs = 2000; // default fade duration
    bool running = false;
    uint16_t startIndex = 0; // first pixel index to affect
    uint16_t length = 0;     // number of pixels to affect

    // begin with optional range: start index and count. If count==0 use all pixels.
    void begin(Adafruit_NeoPixel *t, uint8_t c0[3], uint8_t c1[3], uint8_t c2[3], unsigned long dur, uint16_t startIdx=0, uint16_t cnt=0) {
        target = t;
        durationMs = dur;
        for (int i=0;i<3;i++) { colors[0][i]=c0[i]; colors[1][i]=c1[i]; colors[2][i]=c2[i]; }
        curIdx = 0;
        startMs = millis();
        running = true;
        startIndex = startIdx;
        if (cnt==0) length = target->numPixels() - startIndex; else length = cnt;
    }

    void startNext() {
        startMs = millis();
        // curIdx remains the index of the starting color; next will be (curIdx+1)%3
        running = true;
    }

    void applyColorToTarget(uint8_t r, uint8_t g, uint8_t b) {
        uint16_t n = length;
        for (uint16_t i=0;i<n;i++) {
            uint16_t p = startIndex + i;
            target->setPixelColor(p, target->Color(r,g,b));
        }
        target->show();
    }

    void update() {
        if (!running || target==nullptr) return;
        unsigned long now = millis();
        unsigned long elapsed = now - startMs;
        uint8_t fromIdx = curIdx;
        uint8_t toIdx = (curIdx+1)%3;
        if (elapsed >= durationMs) {
            // finish this fade, set to final color and advance
            applyColorToTarget(colors[toIdx][0], colors[toIdx][1], colors[toIdx][2]);
            curIdx = toIdx;
            // start next fade immediately
            startNext();
        } else {
            float t = (float)elapsed / (float)durationMs;
            int r = (int)(colors[fromIdx][0] + t * (colors[toIdx][0] - colors[fromIdx][0]));
            int g = (int)(colors[fromIdx][1] + t * (colors[toIdx][1] - colors[fromIdx][1]));
            int b = (int)(colors[fromIdx][2] + t * (colors[toIdx][2] - colors[fromIdx][2]));
            applyColorToTarget((uint8_t)r, (uint8_t)g, (uint8_t)b);
        }
    }
};

// Two independent fade tasks: one for the single `pixel`, one for the `strip`.
FadeTask pixelFade;
FadeTask stripFade;

// Icon cycling task (first ICON_COUNT LEDs)
struct IconTask {
    Adafruit_NeoPixel *target = nullptr;
    uint8_t count = 0;
    unsigned long intervalMs = 500;
    unsigned long lastMs = 0;
    int cur = -1;
    uint32_t color = 0;

    void begin(Adafruit_NeoPixel *t, uint8_t iconCount, unsigned long interval, uint32_t col) {
        target = t;
        count = iconCount;
        intervalMs = interval;
        color = col;
        // allow immediate first step
        lastMs = millis() - intervalMs;
        cur = -1;
    }

    void update() {
        if (!target || count == 0) return;
        unsigned long now = millis();
        if (now - lastMs < intervalMs) return;
        int next = (cur + 1) % count;
        if (cur >= 0) target->setPixelColor(cur, target->Color(0,0,0));
        target->setPixelColor(next, color);
        target->show();
        cur = next;
        lastMs = now;
    }

    void setColor(uint8_t r, uint8_t g, uint8_t b) { if (target) color = target->Color(r,g,b); }
};

IconTask iconTask;


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
}

void loop() {
    // update leds
    pixelFade.update();
    stripFade.update();
    // update icon cycler
    iconTask.update();

    // update servos
    servoTask1.update();
    servoTask2.update();
    servoTask3.update();
}