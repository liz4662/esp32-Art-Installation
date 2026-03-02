#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite textStrip = TFT_eSprite(&tft);
TFT_eSprite citySprite = TFT_eSprite(&tft);

static const int H = 175;
static const int W = 240;
static float textX = 150;
static const char* quote = "Every new beginning comes from some other beginning's end. ";

static const int NUM_STARS = 100;
struct Star { uint8_t x, y; } stars[NUM_STARS];
static float cityAlpha = 255.0f;
static float alphaDir = -1.5f;


void drawCity() {
    citySprite.createSprite(W, 60);
    citySprite.fillSprite(TFT_BLACK);

    int buildings[][4] = {
        {0,   20, 20, 40}, {25,  10, 15, 50}, {45,  25, 25, 35},
        {75,   5, 20, 55}, {100, 30, 15, 30}, {120, 15, 30, 45},
        {155, 20, 20, 40}, {180,  8, 25, 52}, {210, 25, 15, 35},
        {230, 18, 10, 42},
    };

    for (auto& b : buildings) {
        citySprite.fillRect(b[0], b[1], b[2], b[3], TFT_WHITE);
        for (int wy = b[1] + 4; wy < b[1] + b[3] - 4; wy += 6)
            for (int wx = b[0] + 3; wx < b[0] + b[2] - 3; wx += 5)
                citySprite.drawPixel(wx, wy, TFT_BLACK);
    }
}

void setup() {
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);

    textStrip.createSprite(150, 23);
    textStrip.setTextSize(3);
    textStrip.setTextColor(TFT_WHITE, TFT_BLACK);

    randomSeed(analogRead(0));
    for (auto& s : stars) {
        s.x = random(0, W);
        s.y = random(0, H);
    }

    drawCity();
}

void loop() {
    float t = millis() / 1000.0f;

    // Stars
    for (int i = 0; i < NUM_STARS; i++) {
        uint8_t bright = 127 + 127 * sinf(t + i * 0.7f);
        tft.drawPixel(stars[i].x, stars[i].y, tft.color565(bright, bright, bright));
    }

    // City
    uint8_t a = (uint8_t)cityAlpha;
    for (int cy = 0; cy < 60; cy++) {
        for (int cx = 0; cx < W; cx++) {
            uint16_t px = citySprite.readPixel(cx, cy);
            if (px != TFT_BLACK) {
                uint8_t r = ((px >> 11) & 0x1F) * a / 255;
                uint8_t g = ((px >> 5)  & 0x3F) * a / 255;
                uint8_t b = ( px        & 0x1F) * a / 255;
                tft.drawPixel(cx, cy + 175, tft.color565(r << 3, g << 2, b << 3));
            } else {
                tft.drawPixel(cx, cy + 175, TFT_BLACK);
            }
        }
    }

    cityAlpha += alphaDir;
    if (cityAlpha <= 0)   { cityAlpha = 0;   alphaDir =  1.5f; }
    if (cityAlpha >= 255) { cityAlpha = 255; alphaDir = -1.5f; }

    int textLen = strlen(quote) * 18;
    textStrip.fillSprite(TFT_BLACK);
    textStrip.setCursor((int)textX, 0);
    textStrip.print(quote);
    textStrip.pushSprite(0, 70);
    textX -= 10.0f;
    if (textX < -textLen) textX = 150;

    delay(33);
}
