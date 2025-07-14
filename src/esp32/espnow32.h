#ifndef ESPNOW32_H
#define ESPNOW32_H
#include "../globals.h"
#include "../func.h"

esp_now_peer_info_t peerInfo;
// Log send results(ESP32 only)
void logSendResult(esp_err_t result)
{
  switch (result)
  {
  case ESP_OK:
    logger.log(LOG_DEBUG, "ESP_OK: Success");
    break;
  case ESP_ERR_ESPNOW_NOT_INIT:
    logger.log(LOG_DEBUG, "ESP_ERR_ESPNOW_NOT_INIT: ESP-NOW is not initialized");
    break;
  case ESP_ERR_ESPNOW_ARG:
    logger.log(LOG_DEBUG, "ESP_ERR_ESPNOW_ARG: Invalid argument");
    break;
  case ESP_ERR_ESPNOW_INTERNAL:
    logger.log(LOG_DEBUG, "ESP_ERR_ESPNOW_INTERNAL: Internal error");
    break;
  case ESP_ERR_ESPNOW_NO_MEM:
    logger.log(LOG_DEBUG, "ESP_ERR_ESPNOW_NO_MEM: Out of memory");
    break;
  case ESP_ERR_ESPNOW_NOT_FOUND:
    logger.log(LOG_DEBUG, "ESP_ERR_ESPNOW_NOT_FOUND: Peer not found");
    break;
  case ESP_ERR_ESPNOW_IF:
    logger.log(LOG_DEBUG, "ESP_ERR_ESPNOW_IF: Interface error");
    break;
  default:
    logger.logf(LOG_DEBUG, "Unknown error: %d\n", result);
    break;
  }
}

// Function for changing peer MAC address(ESP32 only)
bool setEspNowPeer(const uint8_t mac[6], bool enc)
{
  String macStr = macToString(mac);
  logger.logf(LOG_DEBUG, "(%s) ", macStr.c_str());
  if (esp_now_is_peer_exist(mac))
  {
    esp_now_del_peer(mac);
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);

  if (!enc)
  {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      return false;
    }
    return true;
  }
  else
  {
    esp_now_set_pmk((uint8_t *)PRIMARY_MASTER_KEY);
    peerInfo.channel = 0;
    peerInfo.encrypt = true;
    memcpy(peerInfo.lmk, LOCAL_MASTER_KEY, 16);
    memcpy(peerInfo.peer_addr, mac, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      return false;
    }
    return true;
  }
}
// Callback for sending data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  logger.log(LOG_INFO, status == ESP_NOW_SEND_SUCCESS ? "SEND OK" : "SEND FAIL");
}
// Callback for receiving data
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len)
{
  const uint8_t *mac_addr = info->src_addr;
  String mac = macToString(mac_addr);
  String buf = "";
  if (!isEqualMac(peerAddress, mac_addr) && !isEqualMac(defaultAddress, peerAddress) && encENA)
  {
    setEspNowPeer(mac_addr, false);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    esp_now_send(mac_addr, (uint8_t *)NACK_MSG, strlen(NACK_MSG));
    logger.log(LOG_INMSG, "Unknown Peer");
  }
  else
  {
    for (int i = 0; i < len; i++)
    {
      buf += (char)incomingData[i];
    }

    if (!buf.startsWith("ACK") && !buf.startsWith("NACK"))
    {
      logger.logf(LOG_INMSG, "(%s)", mac.c_str());
      for (int i = 0; i < len; i++)
      {
        logger.logChar(LOG_INMSG, incomingData[i]);
#if  !defined(BOARD_ESP01C3) && !defined(BOARD_C6MINI)
        logger2.logChar(LOG_INMSG, incomingData[i]);
#endif
      }
      Serial.println();
      Serial1.println();
      setEspNowPeer(mac_addr, 0);
      vTaskDelay(20 / portTICK_PERIOD_MS);
      esp_err_t result = esp_now_send(mac_addr, (uint8_t *)ACK_MSG, strlen(ACK_MSG));
      logSendResult(result);
    }
    else if (buf.startsWith("ACK"))
    {
      String macLast = macToString(mac_addr);

      if (strncmp((char *)incomingData, ACK_MSG, strlen(ACK_MSG)) == 0)
      {
        logger.log(LOG_ACK, macLast);
      }
      return;
    }
    else
    {
      String macLast = macToString(mac_addr);

      if (strncmp((char *)incomingData, NACK_MSG, strlen(NACK_MSG)) == 0)
      {
        logger.log(LOG_NACK, macLast);
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
  }
}
// Initialize esp-now protocol
void espnowInit()
{
  WiFi.disconnect(true); // Erase credentials and disconnect
  WiFi.mode(WIFI_STA);   // Use only STA mode
  uint8_t staMac[6];
  esp_wifi_get_mac(WIFI_IF_STA, staMac);

  // 3. Set that same MAC on the AP interface
  esp_wifi_set_mac(WIFI_IF_AP, staMac);
  macaddr = WiFi.macAddress();

  logger.logf(LOG_DEBUG, "MAC: %s", macaddr.c_str());

  if (esp_now_init() != ESP_OK)
  {
    logger.log(LOG_ERROR, "ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  if (!encENA)
  {
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      logger.log(LOG_ERROR, "PEER ADD ERROR");
      return;
    }
  }
  else
  {
    esp_now_set_pmk((uint8_t *)PRIMARY_MASTER_KEY);
    peerInfo.channel = 0;
    peerInfo.encrypt = true;
    memcpy(peerInfo.lmk, LOCAL_MASTER_KEY, 16);
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      logger.log(LOG_ERROR, "PEER ADD ERROR");
      return;
    }
  }
}

#endif