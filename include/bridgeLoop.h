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
// Main loop function
String bridgeLoop(Stream &input)
{
    while (input.available())
    {
        char c = input.read();
        if (c == '\n')//end of line reached
        {
            buffer.trim();//remove whitespace
            if (buffer.length() > 0)
            {
                if (buffer.startsWith("BRCMD"))//command received
                {
                    if (buffer.indexOf("+") != -1)
                    {
                        String cmd = buffer.substring(buffer.indexOf("+") + 1, buffer.indexOf(endLine));//extract command
                        handleCommand(cmd);//handle command
                    }
                    else
                    {
                        listAvailableCommands();//list commands if no command specified
                    }
                }
             
                else//normal message
                {

                    if (autoSend)//check if autosend is enabled
                                        {
                    #ifdef ESP32
                                            sendMsg(MSG, (const uint8_t*)peerAddress, &buffer,encENA);//send message
                    #elif defined(ESP8266)
                                            sendMsg(MSG, peerAddress, &buffer);//send message
                    #endif
                                            logger.log(LOG_OUTMSG, buffer);
                    
                                            logger2.log(LOG_OUTMSG, buffer);
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