// ESP8266 Module

#include <Arduino.h>
#include "ESP8266.h"


  ESP8266::ESP8266()
  {
    Head = 0;
    Tail = 0;
    Ok = 0;
    Error = 0;
    Mode = 0;
    memset(Access, 0, sizeof(Access));
    memset(Ssid, 0, sizeof(Ssid));
    memset(Password, 0, sizeof(Password));
    Channel = 0;
    Ecn = 0;
    Connections = 0;
    memset(StationI, 0, sizeof(StationI));
    memset(StationM, 0, sizeof(StationM));
    Dhcp = 0;
    memset(Mac, 0, sizeof(Mac));
    Status = 0;
    Mux = 0;
    IpMode = 0;
    IpTimeOut = 0;
    memset(AIp, 0, sizeof(AIp));
    memset(SIp, 0, sizeof(SIp));
    Transparent = false;
  }

  void ESP8266::doCmd(char *cmd)
  {
    Serial1.println(cmd);
    Tail = Serial1.readBytes(Buffer, 255);
//    Serial.print("<<");Serial.print("(");Serial.print(Tail);Serial.print(")");Serial.print(Buffer);Serial.println(">>");
    parse();
  }
  
  void ESP8266::begin(void)
  {
    delay(1000);
    Serial1.print("+++");
    delay(1000);
    doCmd("AT?");
    if (getError())
    {
      doCmd("AT");
      if (getOk())
      {
        doCmd("AT+CWMODE_CUR?");
        doCmd("AT+CWJAP_CUR?");
        doCmd("AT+CWSAP_CUR?");
        doCmd("AT+CWDHCP_CUR?");
        doCmd("AT+CIPSTAMAC_CUR?");
        // doCmd("AT+CIPSTA_CUR?");
        // doCmd("AT+CIPAP_CUR?");
        doCmd("AT+CIPSTATUS");
        doCmd("AT+CIPMUX?");
        doCmd("AT+CIPMODE?");
        doCmd("AT+CIPSTO?");
        doCmd("AT+CIFSR");
      }
    }
  }

  void ESP8266::parse()
  {
    int i;

    i = Head;
    while (i < Tail)
    {
      if (Buffer[i] > 16)
      {
        if (memcmp(&Buffer[i], "OK", 2) == 0)
        {
            i += 1;
            Ok = 1;
        }
        
        if (memcmp(&Buffer[i], "ERROR", 5) == 0)
        {
            i += 4;
            Error = 1;
        }
        
        if (memcmp(&Buffer[i], "STATUS:", 7) == 0)
        {
            Status = Buffer[i+7] - '0';
        }
        
        if (Buffer[i] == '+')
        {
          if (Buffer[i-1] == 0x0a)
             i = i + doParm(i+1);
        }

        if (Buffer[i] == '>')
        {
          if (Buffer[i-1] == 0x0a)
            Transparent = true;
        }
        
        i++;
      }
      else
        i++;
    }
    Head = 0;
    Tail = 0;
  }

  bool ESP8266::getOk()
  {
    if (Ok == 1)
    {
      Ok = 0;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool ESP8266::getError()
  {
    if (Error == 1)
    {
      Error = 0;
      return true;
    }
    else
    {
      return false;
    }
  }

  char *ESP8266::getAccess()
  {
    return Access;
  }
  
  char *ESP8266::getSsid()
  {
    return Ssid;
  }

  void ESP8266::setSsid(char *c)
  {
    strcpy(Ssid, c);
  }
  
  char *ESP8266::getPassword()
  {
    return Password;
  }

  void ESP8266::setPassword(char *c)
  {
    strcpy(Password, c);
  }
  
  void ESP8266::getMac(char *c)
  {
    strcpy(c, Mac);
  }

  int ESP8266::getStatus()
  {
    return Status;
  }

  int ESP8266::getMode()
  {
    return Mode;
  }
  
  int ESP8266::getIpMode()
  {
    return IpMode;
  }

  char *ESP8266::getAccessIP()
  {
    return AIp;
  }

  char *ESP8266::getStationIP()
  {
    return SIp;
  }

  bool ESP8266::doTransparent()
  {
    doCmd("AT+CIPSEND");
    return Transparent;
  }
  
  int ESP8266::doParm(int p)
  {
    char *current;
    current = &Buffer[p];

    if (memcmp(current, "++", 2) == 0)
      return 2;
      
    if (memcmp(current, "CWMODE_CUR:", 11) == 0)
    {
      Mode = current[11] - '0';
      return 11;
    }
    
    if (memcmp(current, "CWJAP_CUR:", 10) == 0)
    {
      int j = 0;
      int i;
      
      for (i=10;i<Tail;i++)
      {
        if (current[i] == ',')
          break;
        if (current[i] == '"')
          continue;
        Access[j++] = current[i];
      }
      return i;
    }

    if (memcmp(current, "CWSAP_CUR:", 10) == 0)
    {
      int j = 0;
      int k = 0;

      for (int i=10;i<Tail;i++)
      {
        if (current[i] == ',')
        {
          j++;
          k = 0;
          continue;
        }
        if (current[i] == '"')
          continue;
        if (j == 0)
          Ssid[k++] = current[i];
        if (j == 1)
          Password[k++] = current[i];
        if (j == 2)
          Channel = current[i] - '0';
        if (j == 3)
          Ecn = current[i] - '0';
        if (j == 4)
        {
          Connections = current[i] - '0';
          return i;
        }
      }
    }

    if (memcmp(current, "CWLIF", 5) == 0)
    {
      int j = 0;
      int k = 0;

      for (int i=8;i<Tail;i++)
      {
        if (current[i] == ',')
        {
          j++;
          k = 0;
          continue;
        }
        if (*(current+i) == '"')
          continue;
        if (current[i] == 0x0d)
          return i;
        if (j == 0)
          StationI[k++] = current[i];
        if (j == 1)
          StationM[k++] = current[i];
      }
    }
    
    if (memcmp(current, "CWDHCP_CUR:", 11) == 0)
    {
      Dhcp = current[11] - '0';
      return 11;
    }

    if (memcmp(current, "CIPSTAMAC_CUR:", 14) == 0)
    {
      int k = 0;
      int i;
      
      for (i=15;i<Tail;i++)
      {
        if (current[i] == '"')
          break;
        Mac[k++] = current[i];
      }
      return i;
    }

  if (memcmp(current, "CIPSTA:", 7) == 0) //not used
  {
    int k = 0;
    int j = 0;
    int i;
    
    for (int i=7;i<Tail;i++)
    {
      
    }
    return i;
  }

  if (memcmp(current, "CIPMUX:", 7) == 0)
  {
    Mux = current[7] - '0';
    return 7;
  }

  if (memcmp(current, "CIPMODE:", 8) == 0)
  {
    IpMode = current[8] - '0';
    return 8;
  }

  if (memcmp(current, "CIPSTO:", 7) == 0)
  {
    IpTimeOut = atol(&current[7]);
    return 7;
  }

  if (memcmp(current, "CIFSR:APIP,", 11) == 0)
  {
    int k = 0;
    int j = 0;
    int i;
    
    for (i=11;i<Tail;i++)
    {
      if (current[i] == '"')
        continue;
      if (current[i] == 0x0d)
        break;
      AIp[k++] = current[i];
    }
    return i;
  }

  if (memcmp(current, "CIFSR:STAIP,", 12) == 0)
  {
    int k = 0;
    int i;

    for (i=12;i<Tail;i++)
    {
      if (current[i] == '"')
        continue;
      if (current[i] == 0x0d)
        break;
      SIp[k++] = current[i];
    }
    return i;
  }

  return 0;
}

