#ifndef PINS_ESP32C6_H
#define PINS_ESP32C6_H
// GPIO pin definitions for ESP32-C6
const String ESPModel = "ESP32-C6";
const unsigned int GPIO[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 20, 21, 22, 23, 24, 25, 26, 27};
const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
unsigned long startTimeouts[GPIO_COUNT] = {0};
long timeOuts[GPIO_COUNT] = {0};
#endif