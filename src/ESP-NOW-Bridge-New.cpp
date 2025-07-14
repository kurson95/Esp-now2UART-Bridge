#include <Arduino.h>
#include "globals.h"    //gloval variables
#include "func.h"       //global functions
#include "bridgeLoop.h" //main loop function
#if defined(ESP8266)
#include "esp8266/espnow8266.h"
#include "esp8266/commands.h" //commands
#elif defined(ESP32)
#include "esp32/espnow32.h"
#include "esp32/commands32.h" //commands
#endif
void setup()
{
  Serial.begin(BAUD_RATE);
#ifdef ESP8266
  Serial1.begin(BAUD_RATE);
#elif defined(ESP32)
  Serial1.begin(BAUD_RATE, SERIAL_8N1, RX_1, TX_1);
#endif
  Serial.flush();
  Serial.end();
  pinMode(BTN, INPUT_PULLUP);
  prefs.begin("serial", false);
  // Check , if value storing autosend state is in flash
  if (!prefs.isKey("autoSend")) //
  {
    // if not, write it to flash
    prefs.putBool("autoSend", true);
  }
  // Check, if valur storing baud rate is in flash
  if (!prefs.isKey("baudRate"))
  {
    // again,if not, write it to flash
    prefs.putLong("baudRate", 9600);
  }
  // Load stored values and asign then to variables
  autoSend = prefs.getBool("autoSend");
  baudRate = prefs.getLong("baudRate");
  prefs.end();
  Serial.begin(baudRate);
  Serial.flush();
  Serial.println();
  Serial.printf("---| ESP-NOW2UART BRIDGE V0.1.0 |---\nBRCMD+COMMAND=ARG1%sARG2%s\n", argStart, endLine);
  listAvailableCommands();
  // Same procedure as above, but for MAC address and encryption state
  prefs.begin("mac", false);
  if (!prefs.isKey("peerMac"))
  {
    prefs.putBytes("peerMac", defaultAddress, 6);
  }
  prefs.getBytes("peerMac", peerAddress, 6);
  prefs.end();
  prefs.begin("encStat", false);
  if (!prefs.isKey("encENA"))
  {
    prefs.putBool("encENA", false);
  }
  encENA = prefs.getBool("encENA");
  prefs.end();
  // Initialize ESP-NOW protocol.
  espnowInit();
}

void loop()
{

  checkTimeOuts();
  bridgeLoop();
}
