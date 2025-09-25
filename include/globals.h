
#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <HardwareSerial.h>
#include <cstddef>
#include <cstdint>
#define BAUD_RATE 9600
#define BTN 0
#define LED 2
#if defined(ESP8266)
#include <ESP8266WiFi.h>
extern "C"
{
#include <espnow.h>
}
#elif defined(ESP32)
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#endif
#include <Preferences.h>
#include "log.h"
#define ARG_MAX 32
#define MSG_MAX 128
#define MAX_PENDING 10
#define MSG_TIMEOUT 2E3 // milliseconds
unsigned long baudRate;
bool autoSend = true;
HardwareSerial *SerialOut = &Serial;
// HardwareSerial* SerialOut = &Serial1;
// HardwareSerial* SerialOut = &Serial2;

Logger logger(*SerialOut);
static const char endLine[3] = "\r\n";
static const char argStart[2] = ",";
static const char commStart[2] = "=";

QueueHandle_t blinkQueue;

const long allowedBaudRates[] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400, 250000};

Preferences prefs;

uint8_t peerAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t defaultAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const char *PRIMARY_MASTER_KEY = "09%r^ZgTGuL*iGgc";
static const char *LOCAL_MASTER_KEY = "NAqN*HIaqq93!DVm";

bool encENA = false;
char macStr[18];
String macaddr;

enum msgType : uint8_t
{
  NACK,
  ACK,
  MSG,
  CMD,
  PAIR,
  PAIR_ACK,
  PAIR_NACK
} __attribute__((packed));

const char *ACK_MSG = "ACK";
const char *NACK_MSG = "NACK";

#if defined(ESP8266)

#elif defined(ESP32)
const char chipID = (char)(ESP.getEfuseMac() >> 32);

#endif

enum commandType : uint8_t
{
  NONE,
  ADDRECV,
  SHOWMAC,
  REBOOT,
  REMRECV,
  SHOWRECV,
  HELP,
  INFO,
  SEND,
  SETBR,
  SETENC
};

struct commandEntry
{
  const char *name;
  commandType type;
};

const commandEntry commandList[] = {
    {"ADDRECV", ADDRECV},
    {"SHOWMAC", SHOWMAC},
    {"REBOOT", REBOOT},
    {"REMRECV", REMRECV},
    {"SHOWRECV", SHOWRECV},
    {"?", HELP},
    {"INFO", INFO},
    {"SEND", SEND},
    {"SETBR", SETBR},
    {"SETENC", SETENC}};

const size_t commandCount = sizeof(commandList) / sizeof(commandEntry);

typedef struct __attribute__((packed))
{
  uint16_t id;
  bool isBroadcast;
  bool isEncrypted;
  char chipID;
  uint8_t type;
  uint8_t cmd;
  char arg1[ARG_MAX];
  char arg2[ARG_MAX];
  char msgContent[MSG_MAX];
} msgStruct;

typedef struct
{
  uint16_t id;
  unsigned long sentAt;
  bool acked;
} PendingMsg;

PendingMsg pending[MAX_PENDING];

#endif
