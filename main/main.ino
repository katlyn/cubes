#include "Dusk2Dawn.h"
#include "leds.h"
#include "ublox.h"

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
  setColor(0, SUNSET);
  setMode(0, LED_DAYLIGHT);
  setColor(1, 0xFFFF00);
  setMode(1, LED_DAYLIGHT);
}

void loop() {
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000) {
    lastTime = millis(); //Update the timer

    checkUblox();

    Serial.println(getNMEA().isValid());

    float latitude = (float) getNMEA().getLatitude() / 1000000;
    float longitude = (float) getNMEA().getLongitude() / 1000000;

    Serial.println(getNMEA().getLongitude());

    Serial.print("Guessed offset: ");
    Serial.println(guessOffset());

    auto year = getNMEA().getYear();
    auto month = getNMEA().getMonth();
    auto day = getNMEA().getDay();

    Serial.print(year);
    Serial.print(' ');
    Serial.print(month);
    Serial.print(' ');
    Serial.println(day);

    Dusk2Dawn location(latitude, longitude, guessOffset());

    Serial.print(latitude);
    Serial.print(' ');
    Serial.println(longitude);
    Serial.println(location.sunset(year, month, day, false));
    char time[] = "00:00";
    Dusk2Dawn::min2str(time, location.sunset(year, month, day, false));
    Serial.println(time);

    Time localTime;
    getLocalTime(localTime);
    Serial.print(localTime.year);
    Serial.print('-');
    Serial.print(localTime.month);
    Serial.print('-');
    Serial.print(localTime.day);
    Serial.print(' ');
    Serial.print(localTime.hour);
    Serial.print(':');
    Serial.print(localTime.minute);
    Serial.print(':');
    Serial.println(localTime.second);
  }

  updateLeds();
}
