#ifndef PINS_ESP32C3_H
#define PINS_ESP32C3_H
// GPIO pin definitions for ESP32-C3const String ESPModel = "ESP32-C3";
const unsigned int GPIO[] = {0, 1,2,3 ,4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
unsigned long startTimeouts[GPIO_COUNT] = {0};
long timeOuts[GPIO_COUNT] = {0};
#endif