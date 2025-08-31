
#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <HardwareSerial.h>
#include <cstddef>
#include <cstdint>
#define BAUD_RATE 9600
#define BTN 0
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
unsigned long baudRate;
bool autoSend = true;
HardwareSerial* SerialOut = &Serial; 

Logger logger(*SerialOut);

static const char endLine[2] = "\n";
static const char argStart[2] = ",";
static const char commStart[2] = "=";

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
  PAIR
}__attribute__((packed));

const char *ACK_MSG = "ACK";
const char *NACK_MSG = "NACK";

#if defined(ESP8266)
#define TX_1 2

#if defined(ARDUINO_ESP8266_ESP01)
const String ESPModel = "ESP01";
const unsigned int GPIO[] = {0};
#define WIFI_MOD
#elif defined(BOARD_NODEMCU) || defined(ARDUINO_ESP8266_WEMOS_D1MINI) || defined(BOARD_ESP07) || defined(BOARD_ESP12E)
const String ESPModel = "ESP12E/F";
const unsigned int GPIO[] = {4, 5, 12, 13, 14, 15};
#endif



#elif defined(ESP32)

#if defined(CONFIG_IDF_TARGET_ESP32)
const String ESPModel = "ESP32";
const unsigned int GPIO[] = {
    0, 1, 2, 3, 4, 5,
    12, 13, 14, 15,
    18, 19,
    21, 22, 23,
    25, 26, 27,
    32, 33, 34, 35, 36, 37, 38, 39};
#define RX_1 16
#define TX_1 17
#elif defined(BOARD_C6MINI)
const String ESPModel = "ESP32-C6-Mod";
const unsigned int GPIO[] = {0, 1, 2, 18, 19, 20, 21, 22, 23};
#define RX_1 -1
#define TX_1 -1
#elif defined(BOARD_ESP01C3)
const String ESPModel = "ESP01-C3";
const unsigned int GPIO[] = {0};
#define WIFI_MOD
#define RX_1 -1
#define TX_1 -1
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
const String ESPModel = "ESP32-C3";
const unsigned int GPIO[] = {0, 1, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
#define RX_1 2
#define TX_1 3
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
const String ESPModel = "ESP32-C6";
const unsigned int GPIO[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 20, 21, 22, 23, 24, 25, 26, 27};
#define RX_1 18
#define TX_1 19
#else
#error "Unsupported ESP32 variant."
#endif
#endif

const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
unsigned long startTimeouts[GPIO_COUNT] = {0};
long timeOuts[GPIO_COUNT] = {0};

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
  SETGPIO,
  READAN,
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
    {"SETGPIO", SETGPIO},
    {"READAN", READAN},
    {"SETENC", SETENC}};

const size_t commandCount = sizeof(commandList) / sizeof(commandEntry);

typedef struct __attribute__((packed)) {
  uint8_t  type;
  uint8_t  cmd;
  char     arg1[ARG_MAX];
  char     arg2[ARG_MAX];
  char     msgContent[MSG_MAX];
} msgStruct;


#endif
