#include <Arduino.h>
#include <globals.h>    //gloval variables
#include <func.h>       //global functions
#include <bridgeLoop.h> //main loop function
#if defined(ESP8266)
#include "esp8266/espnow8266.h"
#include "esp8266/commands.h" //commands
#elif defined(ESP32)
#include "esp32/espnow32.h"
#include "esp32/commands32.h" //commands
#endif
void setup()
{
  SerialOut->begin(BAUD_RATE);
  SerialOut->flush();
  SerialOut->end();
  pinMode(BTN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
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
  SerialOut->begin(baudRate);
  SerialOut->flush();
  SerialOut->println();
  SerialOut->printf("---| ESP-NOW2UART BRIDGE V0.1.0 |---\nBRCMD+COMMAND=ARG1%sARG2%s\n", argStart, endLine);
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
  xTaskCreate(bridgeLoop, "bridgeLoop", 4096, NULL, 1, NULL);             // main loop
  xTaskCreate(TrackMsgTimeouts, "TrackMsgTimeouts", 2048, NULL, 1, NULL); // check for message timeouts
  blinkQueue = xQueueCreate(5, sizeof(int));
  xTaskCreate(blinkTask, "BlinkTask", 2048, NULL, 1, NULL);
  xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 1, &buttonTaskHandle);
}

void loop()
{
}
