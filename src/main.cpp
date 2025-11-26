#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_R 17
#define RGB_G 16
#define RGB_B 25
#define NEO_PWR 11
#define NEO 12
#define NUMPIXELS 1
#define STRIP 7
#define STRIP_LEN 100


Adafruit_NeoPixel pixel(1, NEO, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_LEN, STRIP, NEO_GRB + NEO_KHZ800);



struct FadeTask {
    Adafruit_NeoPixel *target;
    uint8_t colors[3][3]; // 3-step cycle: each color is {r,g,b}
    uint8_t curIdx = 0;   // index of the "from" color
    unsigned long startMs = 0;
    unsigned long durationMs = 2000; // default fade duration
    bool running = false;

    void begin(Adafruit_NeoPixel *t, uint8_t c0[3], uint8_t c1[3], uint8_t c2[3], unsigned long dur) {
        target = t;
        durationMs = dur;
        for (int i=0;i<3;i++) { colors[0][i]=c0[i]; colors[1][i]=c1[i]; colors[2][i]=c2[i]; }
        curIdx = 0;
        startMs = millis();
        running = true;
    }

    void startNext() {
        startMs = millis();
        // curIdx remains the index of the starting color; next will be (curIdx+1)%3
        running = true;
    }

    void applyColorToTarget(uint8_t r, uint8_t g, uint8_t b) {
        uint16_t n = target->numPixels();
        for (uint16_t p=0;p<n;p++) {
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

    strip.fill(strip.Color(255, 0, 0), 0, STRIP_LEN);
    strip.show();
    delay(200);
    strip.fill(strip.Color(0, 255, 0), 0, STRIP_LEN);
    strip.show();
    delay(200);
    strip.fill(strip.Color(0, 0, 255), 0, STRIP_LEN);
    strip.show();
    delay(200);

    // prepare non-blocking fade tasks
    uint8_t c0[3] = {255, 0, 0};
    uint8_t c1[3] = {0, 255, 0};
    uint8_t c2[3] = {0, 0, 255};
    pixelFade.begin(&pixel, c0, c1, c2, 2000); // 2s fades
    stripFade.begin(&strip, c0, c1, c2, 3000); // 3s fades
}

void loop() {
    pixelFade.update();
    stripFade.update();
}