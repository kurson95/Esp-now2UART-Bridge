#include <sys/stdio.h>
#include <cstring>
#if defined(ESP8266)
#include "core_esp8266_features.h"
#include "esp8266/espnow8266.h"
#elif defined(ESP32)
#include "esp32/espnow32.h"
#endif
#include "WString.h"
#include <sys/_stdint.h>
#ifndef FUNC_H
#define FUNC_H
// gloabl functions
#include "globals.h"
// #include "commands.h"
//Checks if the input matches one of the defined commands
commandType parseCommandFromString(const String &input)
{
  for (size_t i = 0; i < commandCount; i++)
  {
    if (input.startsWith(commandList[i].name))
    {
      return commandList[i].type;
    }
  }
  return NONE;
}
//Checks if the input matches one of the defined baud rates
bool isBaudRateAllowed(long baudRate)
{
  for (long allowedRate : allowedBaudRates)
  {
    if (baudRate == allowedRate)
    {
      return true;
    }
  }
  return false;
}
//GPIO pin control function
void setGPIO(int pin, int state)
{
  bool gpioFound = false;
  for (int io : GPIO)
  {
    if (pin == io)
    {
      gpioFound = true;
      #ifdef ESP8266
      if (pin == 2)
      {
        if (state > 1 && state <= 255)
        {
          state = 255 - state;
        }
        else if (state == 0 || state == 1)
        {
          state = !state;
        }
      }
      #endif
      if (state == 0 || state == 1)
      {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);
        logger.logf(LOG_INFO,"GPIO%d : %d \n", pin, state);
      }
      else if (state > 1 && state <= 255)
      {
        analogWrite(pin, state);
        logger.logf(LOG_INFO,"GPIO%d : %d \n", pin, state);
      }
      else
      {
        logger.logf(LOG_ERROR,"Invalid State");
      }
    }
  }
  if (!gpioFound)
    logger.log(LOG_ERROR,"Invalid Pin");
}
//Overload for GPIO control function. Takes additionall timeout variable
void setGPIO(int pin, int state, long timeOut)
{
  bool gpioFound = false;

  for (int i = 0; i < GPIO_COUNT; i++)
  {
    if (pin == GPIO[i])
    {
      gpioFound = true;
      #ifdef ESP8266
      if (pin == 2)
      {
        if (state > 1 && state <= 255)
        {
          state = 255 - state;
        }
        else if (state == 0 || state == 1)
        {
          state = !state;
        }
      }
      #endif
      if (state == 0 || state == 1)
      {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);
        logger.logf(LOG_INFO,"GPIO%d : %d \n", pin, state);
      }
      else if (state > 1 && state <= 255)
      {
        analogWrite(pin, state);
        logger.logf(LOG_INFO,"GPIO%d : %d \n", pin, state);
      }
      else
      {
        logger.log(LOG_ERROR,"Invalid State");
      }

      if (timeOut > 0)
      {
        timeOuts[i] = timeOut;
        startTimeouts[i] = millis();
      }
    }
    }
  if (!gpioFound)
    logger.log(LOG_ERROR,"Invalid Pin");
}
//Checks, if input is a number
bool isNumber(String str)
{
  str.trim();
  if (str.length() == 0)
    return false;

  int start = 0;
  if (str.charAt(0) == '-' || str.charAt(0) == '+')
  {
    if (str.length() == 1)
      return false; // sam znak + lub - to nie liczba
    start = 1;
  }

  for (int i = start; i < str.length(); i++)
  {
    if (!isDigit(str.charAt(i)))
    {
      return false;
    }
  }
  return true;
}
# ifdef ESP32
//GPIO timeout tracking
void checkTimeOuts()
{
  unsigned long now = millis();
  for (int i = 0; i < GPIO_COUNT; i++)
  {
    if (timeOuts[i] > 0)
    {
      if (now - startTimeouts[i] >= timeOuts[i])
      {
       
          digitalWrite(GPIO[i], LOW);
          logger.logf(LOG_INFO,"GPIO_%d: timeout -> LOW\n", GPIO[i]);
          timeOuts[i] = 0; // wyłącz timeout
        }
      }
    
  }
}
#elif defined(ESP8266)
//GPIO timeout tracking
void checkTimeOuts()
{
  unsigned long now = millis();
  for (int i = 0; i < GPIO_COUNT; i++)
  {
    if (timeOuts[i] > 0)
    {
      if (now - startTimeouts[i] >= timeOuts[i])
      {
        if (GPIO[i] == 2)
        {
          digitalWrite(GPIO[i], HIGH);
          logger.logf(LOG_INFO,"GPIO_%d: timeout -> LOW\n", GPIO[i]);
          timeOuts[i] = 0; // wyłącz timeou
        }
        else
        {
          digitalWrite(GPIO[i], LOW);
          logger.logf(LOG_INFO,"GPIO_%d: timeout -> LOW\n", GPIO[i]);
          timeOuts[i] = 0; // wyłącz timeout
        }
      }
    }
  }
}
//Read analog pin state(ESP8266 only)
int readAGPIO()
{
  pinMode(A0, INPUT);
  int read = analogRead(A0);
  logger.logf(LOG_INFO,"A0: %d \n", read);
  return read;
}
#endif
//Parse MAC string to uint8_t array
bool parseMacAddress(String macStr, uint8_t *mac)
{
  macStr.trim();
  if (macStr.length() != 17)
    return false;

  for (int i = 0; i < 6; i++)
  {
    String byteStr = macStr.substring(i * 3, i * 3 + 2);
    mac[i] = strtoul(byteStr.c_str(), nullptr, 16);
  }
  return true;
}
//Parse uint8_t array to String
String macToString(const uint8_t mac[6])
{
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}
//List all available commands
void listAvailableCommands()
{
  Serial.println("AVAIBLE COMMANDS:");
  for (size_t i = 0; i < commandCount; i++)
  {
    Serial.println(String("- ") + commandList[i].name);
  }
}
//List all available pins
void listAvailablePins()
{
  logger.log(LOG_INFO,"Available Pins: ");
  for (size_t i = 0; i < GPIO_COUNT; i++)
    Serial.print(String(" ") + GPIO[i]);
  Serial.println();
}
//Compare 2 MAC adresses
bool isEqualMac(const uint8_t a[6], const uint8_t b[6]) {
  for (int i = 0; i < 6; ++i) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

#endif