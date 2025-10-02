#ifndef BRIDGE_LOOP_H
#define BRIDGE_LOOP_H
#include <globals.h>
#include <func.h>
#include <cstdint>
#if defined(ESP8266)
#include <esp8266/espnow8266.h>
#include <esp8266/commands.h>
#elif defined(ESP32)
#include <esp32/espnow32.h>
#include <esp32/commands32.h>
#endif
static char inputBuf[128];
static size_t idx = 0;

void IRAM_ATTR onSerial()
{
    while (SerialOut->available())
    {
        char c = SerialOut->read();
        if (idx < sizeof(inputBuf) - 1)
        {
            inputBuf[idx++] = c;
        }
        if (idx >= 2 && inputBuf[idx - 2] == '\r' && inputBuf[idx - 1] == '\n')
        {
            inputBuf[idx] = '\0';
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(lineQueue, inputBuf, &xHigherPriorityTaskWoken);
            if (xHigherPriorityTaskWoken)
                portYIELD_FROM_ISR();
            idx = 0;
        }
    }
}
void bridgeLoop(void *arg)
{

    char rxLine[128];
    for (;;)
    {
        if (xQueueReceive(lineQueue, &rxLine, portMAX_DELAY) == pdTRUE)
        {

            String buffer = String(rxLine);
            buffer.trim(); // remove whitespace
            if (buffer.length() > 0)
            {
                if (buffer.startsWith("BRCMD")) // command received
                {
                    if (buffer.indexOf("+") != -1)
                    {
                        String cmd = buffer.substring(buffer.indexOf("+") + 1, buffer.indexOf(endLine)); // extract command
                        handleCommand(cmd);                                                              // handle command
                    }
                    else
                    {
                        listAvailableCommands(); // list commands if no command specified
                    }
                }
                else // normal message
                {

                    if (autoSend) // check if autosend is enabled
                    {
                        uint16_t Id = generateMessageId();
                        AddPendingMsg(Id);
                        sendMsg(MSG, (const uint8_t *)peerAddress, &buffer, encENA, Id); // send message
                    }
                    else
                    {
                        logger.log(LOG_ERROR, "AutoSend is disabled.");
                    }
                }
            }
            buffer = "";
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

#endif