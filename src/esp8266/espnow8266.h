#ifndef ESPNOW8266_H
#define ESPNOW8266_H
#include "../globals.h"
#include "../func.h"
// Callback for sending data
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  logger.log(LOG_OUTMSG, sendStatus == 0 ? "SEND OK" : "SEND FAIL");
}
// Callback for receiving data
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len)
{
  String mac = macToString(mac_addr);
  String buf = "";
  if (!isEqualMac(peerAddress, mac_addr) && !isEqualMac(defaultAddress, peerAddress) && encENA)
  {
    esp_now_send(mac_addr, (uint8_t *)NACK_MSG, strlen(NACK_MSG));
    logger.log(LOG_INMSG, "Unknown Peer");
  }
  else
  {
    logger.logf(LOG_INMSG, "(%s)", mac.c_str());

    for (int i = 0; i < len; i++)
    {
      buf += (char)incomingData[i];
    }

    if (!buf.startsWith("ACK") && !buf.startsWith("NACK"))
    {
      for (int i = 0; i < len; i++)
      {
        logger.logChar(LOG_INMSG, incomingData[i]);
        #if !defined(ARDUINO_ESP8266_ESP01)
        logger2.logChar(LOG_INMSG, incomingData[i]);
        #endif
      }
      Serial.println();
      esp_now_send(mac_addr, (uint8_t *)ACK_MSG, strlen(ACK_MSG));
    }
    else if (buf.startsWith("ACK"))
    {
      if (strncmp((char *)incomingData, ACK_MSG, strlen(ACK_MSG)) == 0)
      {
        logger.log(LOG_ACK, mac);
      }
      return;
    }
    else
    {
      if (strncmp((char *)incomingData, NACK_MSG, strlen(NACK_MSG)) == 0)
      {
        logger.log(LOG_NACK, mac);
      }
    }

    if (buf.startsWith("SETGPIO"))
    {
      String gpioStr = buf.substring(buf.indexOf("_") + 1, buf.indexOf(commStart));
      String stateStr = buf.substring(buf.indexOf(commStart) + 1, buf.indexOf(endLine));
      gpioStr.trim();
      stateStr.trim();
      int gpio = gpioStr.toInt();
      int state = stateStr.toInt();
      setGPIO(gpio, state);
      if (buf.indexOf(argStart) != -1)
      {
        String timeOutStr = buf.substring(buf.indexOf(argStart) + 1, buf.indexOf(endLine));
        if (isNumber(timeOutStr))
        {
          long timeout = timeOutStr.toInt();
          setGPIO(gpio, state, timeout);
        }
        else
        {
          logger.log(LOG_ERROR, "Invalid argument");
        }
      }
    }
    if (buf.startsWith("READAN"))
    {
      int remoteRead = readAGPIO();
      buffer = "A0: ";
      buffer += remoteRead;
      esp_now_send(mac_addr, (uint8_t *)buffer.c_str(), buffer.length());
      // buffer = strcat("A0", (char *)remoteRead);
    }
  }
}
// Initialize esp-now protocol
void espnowInit()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  WiFi.disconnect();
  macaddr = WiFi.macAddress();

  if (esp_now_init() != 0)
  {
    logger.log(LOG_ERROR, "ESP-NOW INIT ERROR");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Enable both sending and receiving
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  if (encENA)
  {
    esp_now_set_kok((uint8_t *)PRIMARY_MASTER_KEY, 16);
    if (esp_now_add_peer(peerAddress, ESP_NOW_ROLE_COMBO, 1, (uint8_t *)LOCAL_MASTER_KEY, 16) != 0)
    {
      logger.log(LOG_ERROR, "PEER ADD ERROR");
      return;
    }
  }
  else
  {
    if (esp_now_add_peer(peerAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0) != 0)
    {
      logger.log(LOG_ERROR, "PEER ADD ERROR");
      return;
    }
  }
}
#endif