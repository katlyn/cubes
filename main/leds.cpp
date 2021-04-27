#include "leds.h"

#include "Dusk2Dawn.h"
#include "ublox.h"

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
LEDState state[LED_COUNT];

void initLeds() {
  strip.begin();
  strip.clear();
  strip.show();
  for (byte i = 0; i < LED_COUNT; i++) {
    state[i].color = 0xFFFFFF;
    state[i].mode = LED_RAINBOW;
  }
}

void updateLeds () {
  // Since updating the brightness of the LEDs is lossy, do always do it first
  // TODO: This can be a lot more efficient by using a single loop with switch
  // statements.
  autoBrightness();
  solid();
  daylight();
  rainbowCycle();

  // Update all changes
  strip.show();
}

uint32_t color (byte r, byte g, byte b) {
  return strip.Color(r, g, b);
}

void setMode(uint16_t led, LEDMode mode) {
  state[led].mode = mode;
}

void setColor(uint16_t led, uint32_t color) {
  state[led].color = color;
}

LEDMode getMode(uint16_t led) {
  return state[led].mode;
}

uint32_t getColor(uint16_t led) {
  return state[led].color;
}

void autoBrightness () {
  int reading = analogRead(A0);
  // Map numbers to play nicely
  reading = constrain(reading, 0, 500);
  int brightness = map(reading, 0, 500, 10, 254);
  strip.setBrightness(254);
}

// Paints solid as well as blinking LEDs
void solid () {
  static boolean blinkState = true;
  static unsigned long lastUpdate = 0;

  unsigned long elapsedDuration = millis() - lastUpdate;
  if (elapsedDuration > 1000) {
    blinkState = !blinkState;
    lastUpdate = millis();
  }

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    if (state[i].mode == LED_DISABLED) {
      strip.setPixelColor(i, 0);
    } else if (state[i].mode == LED_SOLID) {
      strip.setPixelColor(i, state[i].color);
    } else if (state[i].mode == LED_BLINK) {
      // If we're blinking, blink it
      strip.setPixelColor(i, blinkState ? state[i].color : 0);
    }
  }
}

uint32_t fadeColor (uint32_t from, uint32_t to, int dist, int totalDist) {
  int fromR = from >> 16;
  int fromG = (from >> 8) & 0xFF;
  int fromB = from & 0xFF;

  int toR = to >> 16;
  int toG = (to >> 8) & 0xFF;
  int toB = to & 0xFF;

  byte finalR = fromR + ((toR - fromR) * dist / totalDist);
  byte finalB = fromG + ((toB - fromB) * dist / totalDist);
  byte finalG = fromB + ((toG - fromG) * dist / totalDist);

  return strip.Color(finalR, finalG, finalB);
}

void daylight () {
  if (!getNMEA().isValid()) {
    return;
  }
  Time time;
  getLocalTime(time);
  int localMinutes = (int) time.hour * 60 + time.minute;

  float latitude = (float) getNMEA().getLatitude() / 1000000;
  float longitude = (float) getNMEA().getLongitude() / 1000000;
  Dusk2Dawn zone (latitude, longitude, guessOffset());

  uint32_t color;

  if (time.hour < 12) {
    auto sunrise = zone.sunrise(time.year, time.month, time.day, false);
    if (localMinutes < sunrise) { // We're in early morning
      color = fadeColor(MIDNIGHT, DAWN, localMinutes, sunrise);
    } else {
      auto tilNoon = (zone.sunset(time.year, time.month, time.day, false) - sunrise) / 2;
      color = fadeColor(DAWN, NOON, localMinutes - sunrise, tilNoon);
    }
  } else {
    auto sunset = zone.sunset(time.year, time.month, time.day, false);
    if (localMinutes < sunset) {
      auto sunrise = zone.sunrise(time.year, time.month, time.day, false);
      auto noon = sunrise + (sunset - sunrise) / 2;
      color = fadeColor(NOON, SUNSET, localMinutes - noon, sunset - noon);
    } else {
      color = fadeColor(SUNSET, MIDNIGHT, localMinutes - sunset, 60 * 24 - sunset);
    }
  }

  for (uint16_t i = 0; i <  LED_COUNT; i++) {
    if (state[i].mode == LED_DAYLIGHT) {
      strip.setPixelColor(i, color);
    }
  }
}

void rainbowCycle() {
  static uint16_t iteration = 0;
  static unsigned long lastUpdate = 0;

  unsigned long elapsedDuration = millis() - lastUpdate;
  if (elapsedDuration > 40) {
    iteration += elapsedDuration / 40;
    lastUpdate = millis();
  }
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    if (state[i].mode == LED_RAINBOW) {
      strip.setPixelColor(i, Wheel(((i * 256 / LED_COUNT) + iteration) & 255));
    }
  }
}


// Utility, get the color at the position of the color wheel
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
