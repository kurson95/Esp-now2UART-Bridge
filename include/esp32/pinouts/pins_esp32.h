#ifndef PINS_ESP32_H
#define PINS_ESP32_H
// GPIO pin definitions for ESP32
const String ESPModel = "ESP32";
const unsigned int GPIO[] = {
    0, 1, 2, 3, 4, 5,
    12, 13, 14, 15,16,17,
    18, 19,
    21, 22, 23,
    25, 26, 27,
    32, 33, 34, 35, 36, 37, 38, 39};
const int GPIO_COUNT = sizeof(GPIO) / sizeof(GPIO[0]);
unsigned long startTimeouts[GPIO_COUNT] = {0};
long timeOuts[GPIO_COUNT] = {0};
#endif