// 5x5 WS2812B-V6 RGB LED matrix
// Library: FastLED (install via Arduino Library Manager)

#include <FastLED.h>

// ---------- Settings ----------
#define DATA_PIN     5      // Change to the pin wired to the data resistor
#define MATRIX_W     5
#define MATRIX_H     5
#define NUM_LEDS     (MATRIX_W * MATRIX_H)
#define BRIGHTNESS   80     // 0-255, keep low to limit heat and current
#define MAX_MA       1500   // Power limit in mA (25 LEDs x 60 mA = 1500 max)
#define SERPENTINE   true   // true = rows alternate direction, false = all rows left to right

CRGB leds[NUM_LEDS];

// Convert x (0-4), y (0-4) to LED index. (0,0) is the first LED.
uint16_t XY(uint8_t x, uint8_t y) {
  if (x >= MATRIX_W || y >= MATRIX_H) return NUM_LEDS;  // Out of range
  if (SERPENTINE && (y & 1)) {
    return y * MATRIX_W + (MATRIX_W - 1 - x);
  }
  return y * MATRIX_W + x;
}

// ---------- Startup test: red, green, blue ----------
void startupTest() {
  CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
  for (uint8_t i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, colors[i]);
    FastLED.show();
    delay(400);
  }
  FastLED.clear();
  FastLED.show();
}

// ---------- Animation 1: diagonal rainbow ----------
void rainbowDiagonal() {
  uint8_t t = millis() / 20;
  for (uint8_t y = 0; y < MATRIX_H; y++) {
    for (uint8_t x = 0; x < MATRIX_W; x++) {
      leds[XY(x, y)] = CHSV(t + (x + y) * 20, 255, 255);
    }
  }
}

// ---------- Animation 2: moving dot with fading trail ----------
void movingDot() {
  fadeToBlackBy(leds, NUM_LEDS, 60);
  uint8_t pos = (millis() / 120) % NUM_LEDS;
  leds[pos] = CHSV(millis() / 30, 255, 255);
}

// ---------- Animation 3: plasma ----------
void plasma() {
  uint16_t t = millis() / 10;
  for (uint8_t y = 0; y < MATRIX_H; y++) {
    for (uint8_t x = 0; x < MATRIX_W; x++) {
      uint8_t v = sin8(x * 40 + t) / 2 + sin8(y * 40 + t * 2) / 2;
      leds[XY(x, y)] = CHSV(v + t / 4, 255, 255);
    }
  }
}

// ---------- Main ----------
uint8_t mode = 0;
const uint8_t NUM_MODES = 3;

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MA);
  FastLED.clear();
  FastLED.show();
  startupTest();
}

void loop() {
  // Switch animation every 10 seconds
  EVERY_N_SECONDS(10) {
    mode = (mode + 1) % NUM_MODES;
    FastLED.clear();
  }

  switch (mode) {
    case 0: rainbowDiagonal(); break;
    case 1: movingDot();       break;
    case 2: plasma();          break;
  }

  FastLED.show();
  FastLED.delay(10);  // ~100 fps cap
}
