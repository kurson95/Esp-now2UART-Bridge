#ifndef ESPNOW8266_H
#define ESPNOW8266_H
#include <globals.h>
#include <func.h>

// Callback for sending data
void SendCMD(uint8_t peer[6], commandType cmd, String arg1,
             String arg2 ) {
  msgStruct message ={};
  message.type = (uint8_t)CMD;
  message.cmd = (uint8_t)cmd;
  if (!arg1.isEmpty()) {
    arg1.toCharArray(message.arg1, ARG_MAX);
  } else {
    message.arg1[0] = '\0';
  }

  if (!arg2.isEmpty()) {
    arg2.toCharArray(message.arg2, ARG_MAX);
  } else {
    message.arg2[0] = '\0';
  }

  esp_now_send(peer, (uint8_t *)&message, sizeof(message));
}

void sendMsg(msgType type, uint8_t peer[6], String *input) {
  msgStruct message = {};

  message.type = (uint8_t)type;
  message.cmd = (uint8_t)NONE;
  switch (type) {
  case ACK:
    strncpy(message.msgContent, ACK_MSG, MSG_MAX - 1);
    message.msgContent[MSG_MAX - 1] = '\0';
    break;
  case NACK:
    strncpy(message.msgContent, NACK_MSG, MSG_MAX - 1);
    message.msgContent[MSG_MAX - 1] = '\0';
    break;
  case MSG:
    if (input && !input->isEmpty()) {
      input->toCharArray(message.msgContent, MSG_MAX);
    } else {
      message.msgContent[0] = '\0';
    }
    break;
  default:
    message.msgContent[0] = '\0';
    break;
  }

  esp_now_send(peer, (uint8_t *)&message, sizeof(message));
  logger.log(LOG_DEBUG, message.msgContent);
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  logger.log(LOG_OUTMSG, sendStatus == 0 ? "SEND OK" : "SEND FAIL");
}

// Callback for receiving data
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len)
{
  // zero-init, potem kopiujemy tylko tyle ile jest dostępne
  msgStruct incomingMsg = {};
  int copyLen = len < (int)sizeof(incomingMsg) ? len : (int)sizeof(incomingMsg);
  if (copyLen > 0) {
    memcpy(&incomingMsg, incomingData, copyLen);
  }
  // zabezpieczenie: wymuszenie NUL-terminacji pól
  incomingMsg.arg1[ARG_MAX - 1] = '\0';
  incomingMsg.arg2[ARG_MAX - 1] = '\0';
  incomingMsg.msgContent[MSG_MAX - 1] = '\0';

  String mac = macToString(mac_addr);

  if (!isEqualMac(peerAddress, mac_addr) && !isEqualMac(defaultAddress, peerAddress) && encENA)
  {
    sendMsg(NACK, mac_addr, nullptr);
    logger.log(LOG_INMSG, "Unknown Peer");
  }
  else
  {
    if(incomingMsg.type != ACK && incomingMsg.type != NACK)
      sendMsg(ACK, mac_addr, nullptr);

    logger.logf(LOG_INMSG, "(%s)", mac.c_str());
    switch (incomingMsg.type) {
    case MSG: {
      // msgContent jest teraz char[]
      logger.logf(LOG_INMSG, "(%s) %s", mac.c_str(), incomingMsg.msgContent);
      break;
    }
    case CMD: {
      logger.logf(LOG_DEBUG, "(%s) CMD received: cmd=%d, arg1=%s, arg2=%s", mac.c_str(),
                  incomingMsg.cmd, incomingMsg.arg1, incomingMsg.arg2);
      // przekazujemy const char* (char[]) — dopasuj jeśli funkcja oczekuje Stringów
      handleCommandFromESPNow(incomingMsg.cmd, incomingMsg.arg1, incomingMsg.arg2);
      break;
    }
    case ACK: {
      logger.log(LOG_ACK, mac);
      break;
    }
    case NACK: {
      logger.log(LOG_NACK, mac);
      break;
    }
    default:
      logger.log(LOG_ERROR, "Unknown message type");
      break;
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
