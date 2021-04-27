/*
  Turn on/off various NMEA sentences.
  By: Nathan Seidle
  SparkFun Electronics
  Date: January 3rd, 2019
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to turn on/off the NMEA sentences being output
  over UART1. We use the I2C interface on the u-blox module for configuration
  but you won't see any output from this sketch. You'll need to hook up
  a Serial Basic or other USB to Serial device to UART1 on your u-blox module
  to see the output.

  This example turns off all sentences except for the GPGGA and GPVTG sentences.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/15136
  NEO-M8P RTK: https://www.sparkfun.com/products/15005
  SAM-M8Q: https://www.sparkfun.com/products/15106

  Hardware Connections:
  Plug a Qwiic cable into the GNSS and a RedBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
  Hookup a Serial Basic (https://www.sparkfun.com/products/15096) to UART1 on the u-blox module. Open a terminal at 57600bps
  and see GPGGA and GPVTG sentences.
*/
#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

unsigned long lastGNSSsend = 0;

void setup()
{
  Serial.begin(115200); // Serial debug output over USB visible from Arduino IDE
  Serial.println("Example showing how to enable/disable certain NMEA sentences");

  Wire.begin();

  if (myGNSS.begin() == false)
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  myGNSS.disableNMEAMessage(UBX_NMEA_MSB, COM_PORT_I2C);	//All NMEA enable commands have 0xF0 as MSB
  myGNSS.disableNMEAMessage(UBX_NMEA_DTM, COM_PORT_I2C);	//GxDTM (datum reference)
  myGNSS.disableNMEAMessage(UBX_NMEA_GAQ, COM_PORT_I2C);	//GxGAQ (poll a standard message (if the current talker ID is GA))
  myGNSS.disableNMEAMessage(UBX_NMEA_GBQ, COM_PORT_I2C);	//GxGBQ (poll a standard message (if the current Talker ID is GB))
  myGNSS.disableNMEAMessage(UBX_NMEA_GBS, COM_PORT_I2C);	//GxGBS (GNSS satellite fault detection)
  myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_I2C);	//GxGGA (Global positioning system fix data)
  myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_I2C);	//GxGLL (latitude and long, whith time of position fix and status)
  myGNSS.disableNMEAMessage(UBX_NMEA_GLQ, COM_PORT_I2C);	//GxGLQ (poll a standard message (if the current Talker ID is GL))
  myGNSS.disableNMEAMessage(UBX_NMEA_GNQ, COM_PORT_I2C);	//GxGNQ (poll a standard message (if the current Talker ID is GN))
  myGNSS.disableNMEAMessage(UBX_NMEA_GNS, COM_PORT_I2C);	//GxGNS (GNSS fix data)
  myGNSS.disableNMEAMessage(UBX_NMEA_GPQ, COM_PORT_I2C); //GxGPQ (poll a standard message (if the current Talker ID is GP))
  myGNSS.disableNMEAMessage(UBX_NMEA_GRS, COM_PORT_I2C);	//GxGRS (GNSS range residuals)
  myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_I2C);	//GxGSA (GNSS DOP and Active satellites)
  myGNSS.disableNMEAMessage(UBX_NMEA_GST, COM_PORT_I2C);	//GxGST (GNSS Pseudo Range Error Statistics)
  myGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_I2C);	//GxGSV (GNSS satellites in view)
  myGNSS.enableNMEAMessage(UBX_NMEA_RMC, COM_PORT_I2C);	//GxRMC (Recommended minimum data)
  myGNSS.disableNMEAMessage(UBX_NMEA_TXT, COM_PORT_I2C);	//GxTXT (text transmission)
  myGNSS.disableNMEAMessage(UBX_NMEA_VLW, COM_PORT_I2C);	//GxVLW (dual ground/water distance)
  myGNSS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_I2C);	//GxVTG (course over ground and Ground speed)
  myGNSS.disableNMEAMessage(UBX_NMEA_ZDA, COM_PORT_I2C);	//GxZDA (Time and Date)

  myGNSS.saveConfiguration(); //Optional: Save these settings to NVM

  Serial.println(F("Messages configured."));
}

void loop () {}
