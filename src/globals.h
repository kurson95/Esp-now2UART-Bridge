#include <Arduino.h>
#include <HardwareSerial.h>
#include <sys/_intsup.h>
#include <cstddef>
#include <sys/_stdint.h>
#ifndef GLOBALS_H
#define GLOBALS_H
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
unsigned long baudRate;
// enable or disable autosending when hitting ENTER
bool autoSend = true;
// Initialize logger class
Logger logger(Serial);
//endline symbol
static const char endLine[2] = "\n";
//symbol from which it reads arguments
static const char argStart[2] = ",";
//symbol from which it reads commands
static const char commStart[2] = "=";
// List of allowed baud rates
const long allowedBaudRates[] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 230400, 250000};
// Buffer for storing input strings
static String buffer = "";
Preferences prefs;
// Receiver MAC
uint8_t peerAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Defult broadcast address
uint8_t defaultAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const char *PRIMARY_MASTER_KEY = "09%r^ZgTGuL*iGgc";
static const char *LOCAL_MASTER_KEY = "NAqN*HIaqq93!DVm";
// Enable or disable encryption
bool encENA = false;
// Char array for storing MAC address
char macStr[18];
// MAC address string buffer
String macaddr;
//ack message
const char *ACK_MSG = "ACK";
//nack message
const char *NACK_MSG = "NACK";
#if defined(ESP8266)
// ESP‑01
#define TX_1 2

#if defined(ARDUINO_ESP8266_ESP01)
// ESP-01 pinout
const String ESPModel = "ESP01";
const unsigned int GPIO[] = {0, 2};

#elif defined(BOARD_NODEMCU) || defined(ARDUINO_ESP8266_WEMOS_D1MINI) || defined(BOARD_ESP07) || defined(BOARD_ESP12E)
// ESP12 based MCUs pinout
Logger logger2(Serial1);
const String ESPModel = "ESP12E/F";
const unsigned int GPIO[] = {4, 5, 12, 13, 14, 15};

#endif
// Calculate GPIO count
const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
// Array for storing time, when pin got enabled
unsigned long startTimeouts[GPIO_COUNT] = {0};
// Array for storing GPIO timeouts
long timeOuts[GPIO_COUNT] = {0};
// Enum storing commands
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
// Struct for command
struct commandEntry
{
  const char *name;
  commandType type;
};
// List of enums and corresponding strings
// String response[] = {"MSG_NACK", "MSG_ACK"};
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

#elif defined(ESP32) // Definitions for ESP32-based MCUs

// HardwareSerial SecondSerial1(1);
Logger logger2(Serial1);
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
const unsigned int GPIO[] = {
    0, 1, 2, 18, 19, 20, 21, 22, 23};
#define RX_1 -1
#define TX_1 -1
#elif defined(BOARD_ESP01C3)

const String ESPModel = "ESP01-C3";
const unsigned int GPIO[] = {2, 8, 9};
#define RX_1 -1
#define TX_1 -1
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
const String ESPModel = "ESP32-C3";
const unsigned int GPIO[] = {
    0, 1, 4, 5,
    12, 13, 14, 15, 16, 17,
    18, 19, 20, 21};
#define RX_1 2
#define TX_1 3
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
const String ESPModel = "ESP32-C6";
const unsigned int GPIO[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    20, 21, 22, 23, 24, 25, 26, 27};
    #define RX_1 18
#define TX_1 19
#endif

const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
unsigned long startTimeouts[GPIO_COUNT] = {0};
long timeOuts[GPIO_COUNT] = {0};
// unsigned int GPIO[] = { 2, 4, 5, 12, 13, 14, 15 };
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
  // READAN,
  SETENC
};
struct commandEntry
{
  const char *name;
  commandType type;
};
String response[] = {"MSG_NACK", "MSG_ACK"};
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
    //{ "READAN", READAN },
    {"SETENC", SETENC}};
#endif
// Calculate command count
const size_t commandCount = sizeof(commandList) / sizeof(commandEntry);

#endif