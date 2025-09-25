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
commandType parseCommandFromString(const String &input)
{
  for (size_t i = 0; i < commandCount; i++)
  {
    if (input.startsWith((commandList + i)->name))
    {
      return (commandList + i)->type;
    }
  }
  return NONE;
}
// Checks if the input matches one of the defined baud rates
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
// Checks, if input is a number
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

// Parse MAC string to uint8_t array
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
// Parse uint8_t array to String
String macToString(const uint8_t mac[6])
{
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
          mac[4], mac[5]);
  return String(buf);
}
// List all available commands
void listAvailableCommands()
{
  SerialOut->println("AVAIBLE COMMANDS:");
  for (size_t i = 0; i < commandCount; i++)
  {
    SerialOut->println(String("- ") + commandList[i].name);
  }
}

// Compare 2 MAC adresses
bool isEqualMac(const uint8_t a[6], const uint8_t b[6])
{
  for (int i = 0; i < 6; ++i)
  {
    if (a[i] != b[i])
      return false;
  }
  return true;
}
// Stub for handling commands from ESP-NOW
void handleCommandFromESPNow(commandType cmd, const String arg1, const String arg2)
{
  logger.log(LOG_ERROR, "Command handling from ESP-NOW not implemented yet");
}
uint16_t generateMessageId()
{
  uint64_t chipId = ESP.getEfuseMac();
  uint16_t now = millis();
  uint16_t id = (uint16_t)((chipId >> 16) ^ now);
  return id;
}
void TrackMsgTimeouts(void *arg)
{
  while (1)
  {
    unsigned long now = millis();
    for (int i = 0; i < MAX_PENDING; i++)
    {
      if ((pending + i)->id != 0 && !(pending + i)->acked)
      {
        if (now - (pending + i)->sentAt > MSG_TIMEOUT)
        { // timeout 2s
          logger.logf(LOG_ERROR, "MsgId %lu LOST", (pending + i)->id);
          (pending + i)->id = 0; // clear the slot
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // check every 100ms
  }
}
void AddPendingMsg(uint16_t id)
{
  for (int i = 0; i < MAX_PENDING; i++)
  {
    if ((pending + i)->id == 0)
    { // find empty slot
      (pending + i)->id = id;
      (pending + i)->sentAt = millis();
      (pending + i)->acked = false;
      return;
    }
  }
  int oldestIndex = 0;
  for (int i = 1; i < MAX_PENDING; i++)
  {
    if ((pending + i)->sentAt < (pending + oldestIndex)->sentAt)
    {
      oldestIndex = i;
    }
  }
  (pending + oldestIndex)->id = id;
  (pending + oldestIndex)->sentAt = millis();
  (pending + oldestIndex)->acked = false;
}
void MarkMsgAsAcked(uint16_t id)
{
  for (int i = 0; i < MAX_PENDING; i++)
  {
    if ((pending + i)->id == id)
    {
      (pending + i)->acked = true;
      (pending + i)->id = 0; // clear the slot
      return;
    }
  }
}
void MarkMsgAsNacked(uint16_t id)
{
  for (int i = 0; i < MAX_PENDING; i++)
  {
    if ((pending + i)->id == id)
    {
      (pending + i)->acked = false;
      (pending + i)->id = 0; // clear the slot
      return;
    }
  }
}
void blinkTask(void *param)
{
  unsigned int blinkCount;
  for (;;)
  {
    if (xQueueReceive(blinkQueue, &blinkCount, portMAX_DELAY))
    {
      for (int i = 0; i < blinkCount; i++)
      {
        digitalWrite(LED, HIGH);
        vTaskDelay(pdTICKS_TO_MS(200));
        digitalWrite(LED, LOW);
        vTaskDelay(pdTICKS_TO_MS(200));
      }
    }
  }
}
void startBlink(int times)
{
  xQueueSend(blinkQueue, &times, 0);
}

#endif