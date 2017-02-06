// ESP8266 explorer code
// This code will create a serial relay between the ESP8266 and the Serial Console
// It will also grab some info about the ESP8266 on the way.
// 

#include <Arduino.h>
#include "ESP8266.h"


ESP8266 ESP;
int tm;
bool once;

void setup() {
  int i;
  
  delay(5000);
  
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.setTimeout(250);
  
  ESP.begin();
  Serial.print("SSID: ");Serial.println(ESP.getSsid());
  Serial.print("Access: ");Serial.println(ESP.getAccess());
  Serial.print("Mode: ");Serial.println(ESP.getMode());
  Serial.print("IPMode: ");Serial.println(ESP.getIpMode());
  Serial.print("Status: ");Serial.println(ESP.getStatus());
  Serial.print("AccessIP: ");Serial.println(ESP.getAccessIP());
  Serial.print("StationIP: ");Serial.println(ESP.getStationIP());
  Serial.print("Ready> ");

  // Connect to network and enter transparent connection mode
  ESP.doConnect("Your SSID", "Your Password");
  i = 0;
  tm = millis();
  while (i == 0)
  {
    i = ESP.isConnected();
    if (i == -1)
    {
      Serial.println("Failed");
    }
    if (i == 1)
    {
      if (ESP.doTransparent("9750"))
      {
        tm = millis() - tm;
        Serial.print("Entered Transparent Mode: ");Serial.println(tm);
      }
      else
        Serial.println("Failed");
    }
    if (millis()-tm > 20000)
    {
      Serial.println("Timed Out");
      once = true;
    }
  }
}

void loop() {
  char c;
  
  while (Serial.available() > 0)
  {
    c = Serial.read();
    Serial1.write(c);
  }

  while (Serial1.available() > 0)
  {
    c = Serial1.read();
    if (c < 16)
    {
      Serial.print(" 0");Serial.print(c, HEX);
      if (c == 10)
        Serial.println();
    }
    else
      Serial.write(c);
  }
}
