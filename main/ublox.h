#ifndef UBLOX_H_INCLUDE
#define UBLOX_H_INCLUDE

#ifndef UBX_ADDR // Default ublox address to 0x42
#define UBX_ADDR 0x42
#endif

#include <MicroNMEA.h>
#include <Wire.h>

enum SentenceType {
  NONE = 0,
  NMEA,
  UBX,
  RTCM
};

MicroNMEA & getNMEA();

boolean isUbloxReady (unsigned);
boolean checkUblox();
void processUblox(byte);

int guessOffset();

struct Time {
  short year;
  short month;
  short day;
  short hour;
  short minute;
  short second;
};

void getLocalTime (Time &);

#endif
