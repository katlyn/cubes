#ifndef LEDS_H_INCLUDE
#define LEDS_H_INCLUDE

#include <Adafruit_NeoPixel.h>

#define LED_PIN A3
#define LED_COUNT 2

#define DAWN     0x5FB49C
#define NOON     0xFFFFFF
#define SUNSET   0xF0975C
#define MIDNIGHT 0x0F7173

enum LEDMode {
  LED_DISABLED = 0,
  LED_SOLID,
  LED_BLINK,
  LED_DAYLIGHT,
  LED_RAINBOW
};

struct LEDState {
  LEDMode mode = LED_DISABLED;
  uint32_t color = 0;
};

void initLeds();
void updateLeds();

uint32_t color(byte, byte, byte);

void setMode(uint16_t, LEDMode);
void setColor(uint16_t, uint32_t);

LEDMode getMode(uint16_t);
uint32_t getColor(uint16_t);

void autoBrightness();
void solid();
void daylight();
void rainbowCycle();

uint32_t Wheel(byte);

#endif
