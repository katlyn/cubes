#include "Dusk2Dawn.h"
#include "leds.h"
#include "ublox.h"

#define VERSION 0x00

//Simple local timer. Limits amount if I2C traffic to u-blox module.
long lastTime = 0;

// Variables to track
bool hasFix = false;

void setup() {
  Serial.begin(115200);

  Wire.begin();

  if (!isUbloxReady(3)) {
    // Serial.println("GPS not detected");
    // while (1);
  } else {
    // Serial.println("GPS detected owo");
  }

  initLeds();
}

void loop() {
  if (Serial.available() > 0) {
    byte command = Serial.read();
    byte cube;
    byte mode;
    byte r, g, b;
    uint32_t currentColor;
    // Serial.print("Got cmd ");
    // Serial.print(command, HEX);
    switch (command) {
      // BEGIN
      case (byte)0xBE:
        Serial.write(command);
        Serial.write((byte) VERSION);
        Serial.write(LED_COUNT);
        Serial.write(0xFF);
        break;

      // SET MODE
      case (byte) 0xCB:
        cube = Serial.read();
        mode = Serial.read();
        setMode(cube, static_cast<LEDMode>(mode));
        Serial.write(command);
        break;

      // GET MODE
      case (byte) 0xCC:
        cube = Serial.read();
        Serial.write(command);
        Serial.write(cube);
        Serial.write(getMode(cube));
        break;

      // SET COLOR
      case (byte) 0xCD:
        cube = Serial.read();
        r = Serial.read();
        g = Serial.read();
        b = Serial.read();
        setColor(cube, color(r, g, b));
        Serial.write(command);
        break;
      
      // GET COLOR
      case (byte) 0xCE:
        cube = Serial.read();
        currentColor = getColor(cube);
        r = currentColor >> 16;
        g = (currentColor >> 8) & 0xFF;
        b = currentColor & 0xFF;
        Serial.write(command);
        Serial.write(cube);
        Serial.write(r);
        Serial.write(g);
        Serial.write(b);
        break;

      default:
        Serial.write(0xEE);
    }
  }

  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000) {
    lastTime = millis(); //Update the timer

    checkUblox();

  // Serial.println(getNMEA().isValid());

    float latitude = (float) getNMEA().getLatitude() / 1000000;
    float longitude = (float) getNMEA().getLongitude() / 1000000;

  // Serial.println(getNMEA().getLongitude());

  // Serial.print("Guessed offset: ");
  // Serial.println(guessOffset());

    auto year = getNMEA().getYear();
    auto month = getNMEA().getMonth();
    auto day = getNMEA().getDay();

  // Serial.print(year);
  // Serial.print(' ');
  // Serial.print(month);
  // Serial.print(' ');
  // Serial.println(day);

    Dusk2Dawn location(latitude, longitude, guessOffset());

  // Serial.print(latitude);
  // Serial.print(' ');
  // Serial.println(longitude);
  // Serial.println(location.sunset(year, month, day, false));
    char time[] = "00:00";
    Dusk2Dawn::min2str(time, location.sunset(year, month, day, false));
  // Serial.println(time);

    Time localTime;
    getLocalTime(localTime);
  // Serial.print(localTime.year);
  // Serial.print('-');
  // Serial.print(localTime.month);
  // Serial.print('-');
  // Serial.print(localTime.day);
  // Serial.print(' ');
  // Serial.print(localTime.hour);
  // Serial.print(':');
  // Serial.print(localTime.minute);
  // Serial.print(':');
  // Serial.println(localTime.second);
  }

  updateLeds();
}
