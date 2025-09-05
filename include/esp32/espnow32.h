// #include "../func.h"
// #include "../globals.h"
#ifndef ESPNOW32_H
#define ESPNOW32_H
#include <func.h>
#include <globals.h>

esp_now_peer_info_t peerInfo;
// Log send results(ESP32 only)


// Function for changing peer MAC address(ESP32 only)
bool setEspNowPeer(const uint8_t *peer_addr, bool encrypt) {
    esp_now_peer_info_t peerInfo{};
    memcpy(peerInfo.peer_addr, peer_addr, 6);
    peerInfo.channel = 0;      // 0 = aktualny kanał Wi-Fi
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = encrypt;

    if (encrypt) {
        memcpy(peerInfo.lmk, LOCAL_MASTER_KEY, ESP_NOW_KEY_LEN);  // klucz z globals.h
    }

    // Jeśli peer istnieje, usuń go przed ponownym dodaniem
    if (esp_now_is_peer_exist(peer_addr)) {
        esp_err_t res = esp_now_del_peer(peer_addr);
        if (res != ESP_OK) {
        logger.logf(LOG_ERROR,"Error deleting peer: %s\n", esp_err_to_name(res));
            return false;
        }
    }

    // Dodaj peera z nową konfiguracją
    esp_err_t res = esp_now_add_peer(&peerInfo);
    if (res != ESP_OK) {
        logger.logf(LOG_ERROR,"Error adding peer: %s\n", esp_err_to_name(res));
        return false;
    }

    return true;
}
void SendCMD(const uint8_t peer[6], commandType cmd, String arg1,
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

  esp_err_t result = esp_now_send(peer, (uint8_t *)&message, sizeof(message));
  switch (result) {
  case ESP_OK:
    logger.log(LOG_ERROR, "ESP_OK: Success");
    break;
  case ESP_ERR_ESPNOW_NOT_INIT:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_NOT_INIT: ESP-NOW is not initialized");
    break;
  case ESP_ERR_ESPNOW_ARG:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_ARG: Invalid argument");
    break;
  case ESP_ERR_ESPNOW_INTERNAL:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_INTERNAL: Internal error");
    break;
  case ESP_ERR_ESPNOW_NO_MEM:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_NO_MEM: Out of memory");

    break;
  case ESP_ERR_ESPNOW_NOT_FOUND:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_NOT_FOUND: Peer not found");
    break;
  case ESP_ERR_ESPNOW_IF:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_IF: Interface error");
    break;
  default:
    logger.logf(LOG_ERROR, "Unknown error: %d\n", result);
    break;
  }
}

void sendMsg(msgType type, const uint8_t peer[6], String *input, bool encrypt,uint16_t msgID) {
  setEspNowPeer(peer, encrypt) ? logger.log(LOG_DEBUG, "Temp peer set") : logger.log(LOG_DEBUG, "Temp peer set error");
  msgStruct message = {};
  message.type = (uint8_t)type;
  message.cmd = (uint8_t)NONE;
  message.id = msgID;
switch (type) {
  case ACK:
    strncpy(message.msgContent, ACK_MSG, MSG_MAX - 1);
    break;
  case NACK:
    strncpy(message.msgContent, NACK_MSG, MSG_MAX - 1);
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
  logger.logf(LOG_OUTMSG,"ID:%lu MSG: %s",message.id ,String(message.msgContent));
  setEspNowPeer(peerAddress, encENA) ? logger.log(LOG_DEBUG, "Peer restored") : logger.log(LOG_DEBUG, "Peer restore error");
  esp_err_t result;
  result = esp_now_send(peer, (uint8_t *)&message, sizeof(message));
  switch (result) {
  case ESP_OK:
    logger.log(LOG_INFO, "ESP_OK: Success");
    break;
  case ESP_ERR_ESPNOW_NOT_INIT:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_NOT_INIT: ESP-NOW is not initialized");
    break;
  case ESP_ERR_ESPNOW_ARG:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_ARG: Invalid argument");
    break;
  case ESP_ERR_ESPNOW_INTERNAL:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_INTERNAL: Internal error");
    break;
  case ESP_ERR_ESPNOW_NO_MEM:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_NO_MEM: Out of memory, retrying...");
    delay(10);
    result = esp_now_send(peer, (uint8_t *)&message, sizeof(message));
    break;
  case ESP_ERR_ESPNOW_NOT_FOUND:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_NOT_FOUND: Peer not found");
    break;
  case ESP_ERR_ESPNOW_IF:
    logger.log(LOG_ERROR, "ESP_ERR_ESPNOW_IF: Interface error");
    break;
  default:
    logger.logf(LOG_ERROR, "Unknown error: %d\n", result);
    break;
  }
}



// Callback for sending data
void OnDataSent(const wifi_tx_info_t *mac_addr, esp_now_send_status_t status) {
  logger.log(LOG_INFO,
             status == ESP_NOW_SEND_SUCCESS ? "SEND OK" : "SEND FAIL");
}
// Callback for receiving data
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData,
                int len) {
  msgStruct incomingMsg = {};
  int copyLen = len < (int)sizeof(msgStruct) ? len : (int)sizeof(msgStruct);
  memcpy(&incomingMsg, incomingData, copyLen); 
  uint16_t IncomingMsgID = incomingMsg.id;
  incomingMsg.arg1[ARG_MAX - 1] = '\0';
  incomingMsg.arg2[ARG_MAX - 1] = '\0';
  incomingMsg.msgContent[MSG_MAX - 1] = '\0';
  const uint8_t *mac_addr = info->src_addr;
    bool eqPeer_addr = isEqualMac(peerAddress, mac_addr);
    bool eqDef_addr = isEqualMac(defaultAddress, mac_addr);
  String mac = macToString(mac_addr);
  logger.logf(LOG_DEBUG, "Received %d bytes from %s , Id: %lu", len, mac.c_str(),IncomingMsgID);
  if (!eqPeer_addr && !eqDef_addr && encENA) {
  // esp_now_send(mac_addr, (uint8_t *)NACK_MSG, strlen(NACK_MSG));
  sendMsg(NACK, (const uint8_t *)mac_addr, nullptr, false,IncomingMsgID);
  logger.log(LOG_INMSG, "Unknown Peer");
  } else {
    if(incomingMsg.type != ACK && incomingMsg.type != NACK)
      eqPeer_addr ? sendMsg(ACK,(const uint8_t*)mac_addr,nullptr,true,IncomingMsgID) : sendMsg(ACK, (const uint8_t*)mac_addr, nullptr,false,IncomingMsgID);

    switch (incomingMsg.type) {
    case MSG: {
      String txt = String(incomingMsg.msgContent);
    logger.logf(LOG_INMSG, "[%s] %s", mac.c_str(), txt.c_str());
    break;
    }
    case CMD: {
      logger.logf(LOG_CMD, "[%s] CMD received: cmd=%d, arg1=%s, arg2=%s",
                mac.c_str(), incomingMsg.cmd, incomingMsg.arg1, incomingMsg.arg2);
      break;
    }
    case ACK: {
      logger.logf(LOG_ACK,"[%s] ID: %lu" ,mac.c_str(), IncomingMsgID);
      MarkMsgAsAcked(IncomingMsgID);
    } break;
    case NACK: {
      logger.logf(LOG_NACK,"[%s] ID: %lu" ,mac.c_str(), IncomingMsgID);
      MarkMsgAsNacked(IncomingMsgID);
    }
    default:
      logger.log(LOG_ERROR, "Unknown message type");
      break;
    }
  }
}

// Initialize esp-now protocol
void espnowInit() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  delay(100); 
  uint8_t staMac[6];
  if (esp_wifi_get_mac(WIFI_IF_STA, staMac) != ESP_OK) {
    logger.log(LOG_ERROR, "Failed to get STA MAC");
    return;
  }

  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          staMac[0], staMac[1], staMac[2], staMac[3], staMac[4], staMac[5]);
  macaddr = String(buf);

  logger.logf(LOG_INFO, "STA MAC: %s", macaddr.c_str());

  // uint8_t apMac[6];
  // memcpy(apMac, staMac, 6);
  // apMac[5] += 1;  
  // if (esp_wifi_set_mac(WIFI_IF_AP, apMac) != ESP_OK) {
  //   logger.log(LOG_ERROR, "Failed to set AP MAC");
  // }

  if (esp_now_init() != ESP_OK) {
    logger.log(LOG_ERROR, "ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Konfiguracja peerów
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;

  if (!encENA) {
    peerInfo.encrypt = false;
  } else {
    esp_now_set_pmk((uint8_t *)PRIMARY_MASTER_KEY);
    peerInfo.encrypt = true;
    memcpy(peerInfo.lmk, LOCAL_MASTER_KEY, 16);
  }

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    logger.log(LOG_ERROR, "PEER ADD ERROR");
    return;
  }

  logger.log(LOG_INFO, "ESP-NOW ready");
}


#endif