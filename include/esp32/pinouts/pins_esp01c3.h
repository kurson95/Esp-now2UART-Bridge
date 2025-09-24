#ifndef PINS_ESP01C3_H
#define PINS_ESP01C3_H
// GPIO pin definitions for ESP01-C3
#define PINS_ESP32C6MOD_H
// GPIO pin definitions for ESP32-C6-Mod
const String ESPModel = "ESP01-C3";
const unsigned int GPIO[] = {0};
#define WIFI_MOD
const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
unsigned long startTimeouts[GPIO_COUNT] = {0};
long timeOuts[GPIO_COUNT] = {0};
#endif