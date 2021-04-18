#include "ublox.h"

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

MicroNMEA & getNMEA () {
  return nmea;
}

boolean isUbloxReady(unsigned tries) {
  for (unsigned i = 0; i < tries; i++) {
    Wire.beginTransmission(UBX_ADDR); //Connect to the u-blox module using Wire port
    if (Wire.endTransmission() != 0) {
      return true;
    }
    delay(5);
  }
  return false;
}

boolean checkUblox() {
  uint16_t bytesAvailable = 0;
  Wire.beginTransmission(UBX_ADDR);
  Wire.write(0xFD); // Register that contains bytes of available
  if (Wire.endTransmission(false) != 0) { // Send restart, don't release bus.
    // Serial.println("Didn't ACK");
    return false; // Didn't AKC
  }

  Wire.requestFrom(UBX_ADDR, 2);
  if (Wire.available()) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();

    if (lsb == 0xFF) { // Ublox error, should never return 0xFF
      // Serial.println("0xFF");
      return false;
    }

    bytesAvailable = msb << 8 | lsb;
  }

  // Serial.print(bytesAvailable);
  // Serial.println(" bytes available from ubx");

  while (bytesAvailable) {
    Wire.beginTransmission(UBX_ADDR);
    Wire.write(0xFF); // Register to read data from
    if (Wire.endTransmission(false) !=0) { // Send restart, don't release bus.
      return false; // Didn't ACK
    }

    //Limit to 32 bytes or whatever the buffer limit is for given platform
    uint16_t bytesToRead = bytesAvailable;
    if (bytesToRead > 32) {
      bytesToRead = 32;
    }

    Wire.requestFrom(UBX_ADDR, bytesToRead);
    if (Wire.available()) {
      for (uint16_t x = 0; x < bytesToRead; x++) {
        byte incoming = Wire.read();

        if (x == 0 && incoming == 0x7F) { // GPS not ready
          // Serial.println("gps not ready");
          delay(3); // Wait a small bit
          x--;
          continue;
        }
        processUblox(incoming);
      }
    }
    bytesAvailable -= bytesToRead;
  }
  return true;
}

SentenceType currentSentence = NONE;
void processUblox(byte incoming) {
  if ((currentSentence == NONE) || (currentSentence == NMEA)) {
    if (incoming == 0xB5) { //UBX binary frames start with 0xB5, aka μ
      //This is the start of a binary sentence. Reset flags.
      //We still don't know the response class
      currentSentence = UBX;
    } else if (incoming == '$') {
      currentSentence = NMEA;
    } else if (incoming == 0xD3) { //RTCM frames start with 0xD3
      currentSentence = RTCM;
    } else {
      //This character is unknown or we missed the previous start of a sentence
    }
  }

  if (currentSentence == NMEA) {
    nmea.process(incoming);
  }
}

// Estimates the current timezone offset based off of current location
int guessOffset () {
  return nmea.getLongitude() / 1000000 * 12 / 180;
}

void getLocalTime (Time & time) {
  auto offset = guessOffset();
  time.year = nmea.getYear();
  time.month = nmea.getMonth();
  time.day = nmea.getDay();
  time.hour = nmea.getHour() + offset;
  time.minute = nmea.getMinute();
  time.second = nmea.getSecond();

  if (time.hour > 23) {
    time.hour -= 23;
    time.day++;
  } else if (time.hour < 0) {
    time.hour += 23;
    time.day--;
  }
}
