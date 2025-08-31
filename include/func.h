#ifndef FUNC_H
#define FUNC_H
#include <log.h>
#include <cstring>
#include <sys/stdio.h>
// #if defined(ESP8266)
// #include "core_esp8266_features.h"
// #include "../esp8266/espnow8266.h"
// #elif defined(ESP32)
// #include <esp32/espnow32.h>
// #endif
#include "WString.h"
#include <sys/_stdint.h>

// gloabl functions
#include <globals.h>
// #include "commands.h"
#include "WString.h"
#include <sys/_stdint.h>
// Checks if the input matches one of the defined commands
commandType parseCommandFromString(const String &input) {
  for (size_t i = 0; i < commandCount; i++) {
    if (input.startsWith((commandList+i) ->name)) {
      return (commandList+i)->type;
    }
  }
  return NONE;
}
// Checks if the input matches one of the defined baud rates
bool isBaudRateAllowed(long baudRate) {
  for (long allowedRate : allowedBaudRates) {
    if (baudRate == allowedRate) {
      return true;
    }
  }
  return false;
}
// GPIO pin control function
void setGPIO(int pin, int state) {
  #ifdef WIFI_MOD
    logger.log(LOG_ERROR, "GPIO control not supported on WiFi modules");
    return;
  #else
  bool gpioFound = false;
  for (int io : GPIO) {
    if (pin == io) {
      gpioFound = true;

      if (state == 0 || state == 1) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);
        logger.logf(LOG_INFO, "GPIO%d : %d \n", pin, state);
      } else if (state > 1 && state <= 255) {
        analogWrite(pin, state);
        logger.logf(LOG_INFO, "GPIO%d : %d \n", pin, state);
      } else {
        logger.logf(LOG_ERROR, "Invalid State");
      }
    }
  }
  if (!gpioFound)
    logger.log(LOG_ERROR, "Invalid Pin");
  #endif
}
// Overload for GPIO control function. Takes additionall timeout variable
void setGPIO(int pin, int state, long timeOut) {
  #ifdef WIFI_MOD
    logger.log(LOG_ERROR, "GPIO control not supported on WiFi modules");
    return;
  #else
  bool gpioFound = false;
    logger.log(LOG_ERROR, "Invalid Pin");

  for (int i = 0; i < GPIO_COUNT; i++) {
    if (pin == GPIO[i]) {
      gpioFound = true;

      if (state == 0 || state == 1) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);
        logger.logf(LOG_INFO, "GPIO%d : %d \n", pin, state);
      } else if (state > 1 && state <= 255) {
        analogWrite(pin, state);
        logger.logf(LOG_INFO, "GPIO%d : %d \n", pin, state);
      } else {
        logger.log(LOG_ERROR, "Invalid State");
      }
      if (timeOut > 0) {
        timeOuts[i] = timeOut;
        startTimeouts[i] = millis();
      }
    }
  }
  if (!gpioFound)
    logger.log(LOG_ERROR, "Invalid Pin");
  #endif
}
// Checks, if input is a number
bool isNumber(String str) {
  str.trim();
  if (str.length() == 0)
    return false;

  int start = 0;
  if (str.charAt(0) == '-' || str.charAt(0) == '+') {
    if (str.length() == 1)
      return false; // sam znak + lub - to nie liczba
    start = 1;
  }

  for (int i = start; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}
void toggleGPIO(int pin) {
  pinMode(pin, OUTPUT); // upewnij się, że pin jest wyjściem
  int currentState = digitalRead(pin);
  digitalWrite(pin, !currentState);
  logger.logf(LOG_INFO, "GPIO%d : toggled to %d\n", pin, digitalRead(pin));
}
// GPIO timeout tracking
void checkTimeOuts() {
  unsigned long now = millis();
  for (int i = 0; i < GPIO_COUNT; i++) {
    if (timeOuts[i] > 0) {
      if (now - startTimeouts[i] >= timeOuts[i]) {
        
        toggleGPIO(GPIO[i]);
        logger.logf(LOG_INFO, "GPIO_%d: timeout -> %d\n", GPIO[i], digitalRead(GPIO[i]));
        timeOuts[i] = 0; // wyłącz timeout
      }
    }
  }
}
// Read analog pin state(ESP8266 only)
int readAGPIO() {

  pinMode(A0, INPUT);
  int read = analogRead(A0);
  logger.logf(LOG_INFO, "A0: %d \n", read);
  return read;
  
}
// Parse MAC string to uint8_t array
bool parseMacAddress(String macStr, uint8_t *mac) {
  macStr.trim();
  if (macStr.length() != 17)
    return false;

  for (int i = 0; i < 6; i++) {
    String byteStr = macStr.substring(i * 3, i * 3 + 2);
    mac[i] = strtoul(byteStr.c_str(), nullptr, 16);
  }
  return true;
}
// Parse uint8_t array to String
String macToString(const uint8_t mac[6]) {
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
          mac[4], mac[5]);
  return String(buf);
}
// List all available commands
void listAvailableCommands() {
  SerialOut->println("AVAIBLE COMMANDS:");
  for (size_t i = 0; i < commandCount; i++) {
    SerialOut->println(String("- ") + commandList[i].name);
  }
}
// List all available pins
void listAvailablePins() {
  String pins = "Available Pins:";
  for (size_t i = 0; i < GPIO_COUNT; i++) {
    pins += " " + String(GPIO[i]);
  }
  logger.log(LOG_INFO, pins.c_str());
}
// Compare 2 MAC adresses
bool isEqualMac(const uint8_t a[6], const uint8_t b[6]) {
  for (int i = 0; i < 6; ++i) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
// Stub for handling commands from ESP-NOW
void handleCommandFromESPNow(commandType cmd, const String arg1, const String arg2) {
  logger.log(LOG_ERROR, "Command handling from ESP-NOW not implemented yet");
}

#endif