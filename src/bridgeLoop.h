#ifndef BRIDGE_LOOP_H
#define BRIDGE_LOOP_H
#include "globals.h"
#include "func.h"
#if defined(ESP8266)
#include "esp8266/espnow8266.h"
#include "esp8266/commands.h"
#elif defined(ESP32)
#include "esp32/espnow32.h"
#include "esp32/commands32.h"
#endif
// Main loop function
String bridgeLoop()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            buffer.trim();
            if (buffer.length() > 0)
            {
                if (buffer.startsWith("BRCMD"))
                {
                    if (buffer.indexOf("+") != -1)
                    {
                        String cmd = buffer.substring(buffer.indexOf("+") + 1, buffer.indexOf(endLine));
                        handleCommand(cmd);
                    }
                    else
                    {
                        listAvailableCommands();
                    }
                }
                else if (buffer.startsWith("ACK") || buffer.startsWith("NACK"))
                {
                    logger.log(LOG_ERROR, "INVALID SYNTAX");
                }
                else
                {

                    if (autoSend)
                    {
#ifdef ESP8266
                        esp_now_send(peerAddress, (uint8_t *)buffer.c_str(), buffer.length());
#elif defined(ESP32)
                        esp_err_t result = esp_now_send(peerAddress, (uint8_t *)buffer.c_str(), buffer.length());
                        logSendResult(result);
#endif
                        logger.log(LOG_OUTMSG, buffer);
                        #if !defined(ARDUINO_ESP8266_ESP01) && !defined(ARDUINO_ESP32_C6MINI) && !defined(ARDUINO_ESP32_ESP01C3)

                        logger2.log(LOG_OUTMSG, buffer);
                        #endif  
                    }
                    else
                    {
                        logger.log(LOG_ERROR, "AutoSend is disabled.");
                    }
                }
            }
            buffer = "";
        }
        else
        {
            buffer += c;
        }
    }
    return buffer;
}
#endif