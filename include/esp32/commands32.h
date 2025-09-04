#include <cstdlib>
#ifndef COMMANDS32_H
#define COMMANDS32_H
#include <globals.h>
#include <func.h>
#include "espnow32.h"

// Saves MAC address to the flash
void addrecv(uint8_t *mac)
{
  prefs.begin("mac", false);
  prefs.putBytes("peerMac", mac, 6);
  prefs.end();
  // logger.log(LOG_INFO,macStr);
}
// Rebot the MCU
void reboot()
{
  logger.log(LOG_INFO, "REBOOTING...");
  SerialOut->flush();
  SerialOut->end();
  delay(1E3);
  ESP.restart();
}
// Saves encryption state
void setENC(bool enc)
{
  prefs.begin("encStat", false);
  prefs.putBool("encENA", enc);
  prefs.end();
  encENA = enc;
  //delay(100);
  espnowInit();

  // reboot();
}
// Deletes receiver MAC address,and restores the default one
void deletePeer()
{
  uint8_t delStatus = esp_now_del_peer(peerAddress);
  prefs.begin("mac", false);
  prefs.putBytes("peerMac", defaultAddress, 6);
  memcpy(peerAddress, defaultAddress, 6);
  prefs.end();
  if (delStatus == 0)
  {
    setEspNowPeer(peerAddress,0);
    // logger.log(LOG_INFO, "OK");
  }
  else
  {
    logger.log(LOG_ERROR, "Failed to delete peer");
  }
}
// Saves auto-send variable
void setAutoSend(bool state)
{
  prefs.begin("serial", false);
  prefs.putBool("autoSend", state);
  prefs.end();
  autoSend = state;
}
// Checks if the input data matches the defined variable baud rate, and if so, writes it to flash memory
void setBaudRate(long baud)
{

  if (isBaudRateAllowed(baud))
  {
    prefs.begin("serial", false);
    prefs.putLong("baudRate", baud);
    prefs.end();
    reboot();
  }
  else
  {
    logger.log(LOG_ERROR, "Invalid argument");
  }
}
void printSystemInfo()
{
  String info;

  info += "===== System Info =====\n";
  info += "Platform: ESP32\n";
  info += "CPU freq: " + String(getCpuFrequencyMhz()) + " MHz\n";
  info += "Chip model: " + String(ESP.getChipModel()) + endLine;
  info += "Chip revision: " + String(ESP.getChipRevision()) + endLine;
  info += "Chip cores: " + String(ESP.getChipCores()) + endLine;
  info += "Flash size: " + String(ESP.getFlashChipSize()) + " bytes\n";
  info += "Free heap: " + String(ESP.getFreeHeap()) + " bytes\n";
  info += "Min heap (since boot): " + String(esp_get_minimum_free_heap_size()) + " bytes\n";
  info += "Sketch size: " + String(ESP.getSketchSize()) + " bytes\n";
  info += "Free sketch space: " + String(ESP.getFreeSketchSpace()) + " bytes\n";
  info += "SDK version: " + String(ESP.getSdkVersion()) + endLine;
  info += "MAC Address: " + macaddr + endLine;
  info += "Encryption: " + String(encENA ? "On" : "Off") + endLine;
  info += "TypeToSend: " + String(autoSend ? "Enabled" : "Disabled") + endLine;
  info += "Baud Rate: " + String(baudRate) + endLine;
  info += "Peer MAC: " + macToString(peerAddress) + endLine;
  info += "GPIO count: " + String(GPIO_COUNT) + endLine;
  info += "========================\n";

  logger.log(LOG_NONE, info);
}

// Handle commands
void handleCommand(String input)
{
  commandType cmd = parseCommandFromString(input);

  switch (cmd)
  {
  case ADDRECV:
  {
    logger.log(LOG_CMD,input);
    String subcmd = input.substring(input.indexOf(commStart) + 1, input.indexOf(endLine));
    if (!parseMacAddress(subcmd, peerAddress))
    {
      logger.log(LOG_ERROR, "Invalid MAC");
    }
    else
    {
      // logger.log(LOG_INFO,"OK");
     // esp_now_del_peer(peerAddress);
      logger.log(LOG_INFO, macToString(peerAddress));
      addrecv(peerAddress);
      bool addstat = setEspNowPeer(peerAddress,1);
      logger.log(LOG_INFO, addstat ? "OK" : "ERROR");
      setENC(1);
    }
    break;
  }
  case SHOWMAC:
  {
    logger.log(LOG_CMD,input);
    // logger.log(LOG_INFO, "OK");
    logger.log(LOG_INFO, "MAC: " + macaddr);
    // Odczytaj i wypisz MAC
    break;
  }
  case REMRECV:
  {
    logger.log(LOG_CMD,input);
    deletePeer();
    setENC(0);
    logger.log(LOG_INFO,"OK");
    break;
  }
  case SHOWRECV:
  {
    logger.log(LOG_CMD,input);
    // logger.log(LOG_INFO, "OK");
    logger.log(LOG_INFO, "RECV: " + macToString(peerAddress));
    break;
  }
  case REBOOT:
  {
    logger.log(LOG_CMD,input);
    // logger.log(LOG_INFO, "OK");
    reboot();
    break;
  }
  case HELP:
  {
    logger.log(LOG_CMD,input);
    // logger.log(LOG_INFO, "OK");
    listAvailableCommands();
    break;
  }
  case INFO:
  {
    logger.log(LOG_CMD,input);
    // logger.log(LOG_INFO, "OK");
    printSystemInfo();
    break;
  }
  case SEND:
  {
    logger.log(LOG_CMD,input);
    String subcmd = input.substring(input.indexOf(commStart) + 1, input.indexOf(endLine));
    if (subcmd.equals("0"))
    {
      setAutoSend(0);
      // logger.log(LOG_INFO, "OK");
    }
    else if (subcmd.equals("1"))
    {
      setAutoSend(1);
      // logger.log(LOG_INFO, "OK");
    }
    else if (input.indexOf(argStart) != -1)
    {
      uint8_t tempMac[6];
      String mac = input.substring(input.indexOf(argStart) + 1, input.indexOf(endLine));
      if (!parseMacAddress(mac, tempMac))
      {
        logger.log(LOG_ERROR, "Invalid MAC");
      }
      else
      {
        bool addstat = setEspNowPeer(tempMac,0);
        logger.log(LOG_INFO, addstat ? "OK" : "ERROR");
        String msg = input.substring(input.indexOf(commStart) + 1, input.indexOf(argStart));
        //esp_err_t result = esp_now_send(tempMac, (uint8_t *)msg.c_str(), msg.length());
        uint16_t msgID = generateMessageId();
        sendMsg(MSG, (const uint8_t *)tempMac, &msg,false,msgID);
        logger.log(LOG_OUTMSG, "SEND: " + msg);
      }
    }

    else
    {
      esp_err_t result = esp_now_send(peerAddress, (uint8_t *)subcmd.c_str(), subcmd.length());
      logger.log(LOG_OUTMSG, "SEND: " + subcmd);
      logSendResult(result);
    }
    break;
  }
  case SETBR:
  {
    logger.log(LOG_CMD,input);
    String baudString = input.substring(input.indexOf(commStart) + 1, input.indexOf(endLine));
    baudString.trim();
    long br = baudString.toInt();
    logger.logf(LOG_INFO, "Baud rate = %ld", br);
    setBaudRate(br);
    break;
  }
  case SETGPIO:
  {
    logger.log(LOG_CMD,input);
    String gpioStr = input.substring(input.indexOf("_") + 1, input.indexOf(commStart));
    String stateStr = input.substring(input.indexOf(commStart) + 1, input.indexOf(endLine));
    gpioStr.trim();
    stateStr.trim();
    if (input.indexOf("_") != -1 && isNumber(gpioStr))
    {

      int gpio = gpioStr.toInt();
      int state = stateStr.toInt();

      if (input.indexOf(argStart) != -1)
      {
        String timeOutStr = input.substring(input.indexOf(argStart) + 1, input.indexOf(endLine));
        if (isNumber(timeOutStr))
        {
          long timeout = timeOutStr.toInt();
          setGPIO(gpio, state, timeout);
          break;
        }
        else
        {
          logger.log(LOG_ERROR, "Invalid argument");
          break;
        }
      }
      else
      {
        setGPIO(gpio, state);
        break;
      }
    }
    else
      listAvailablePins();
    break;
  }
  case READAN:
  {
    logger.log(LOG_ERROR, "Analog read not supported on ESP32 modules yet");
    break;
  }
  case SETENC:
  {
    logger.log(LOG_CMD,input);
    String subcmd = input.substring(input.indexOf(commStart) + 1, input.indexOf(endLine));

    if (subcmd.equals("0"))
    {
      logger.log(LOG_INFO, "OK");
      setENC(false);
    }
    else if (subcmd.equals("1"))
    {
      logger.log(LOG_INFO, "OK");
      setENC(true);
    }
    else
    {
      logger.log(LOG_ERROR, "Invalid argument");
    }
  }
    // default:
    // {
    //   logger.log(LOG_ERROR,"Cmd unknown");
    // }
  }
}

#endif