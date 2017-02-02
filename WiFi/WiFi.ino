// ESP8266 explorer code
// This code will create a serial relay between the ESP8266 and the Serial Console
// It will also grab some info about the ESP8266 on the way.
// 

#include <Arduino.h>
#include "ESP8266.h"


ESP8266 ESP;

void setup() {
  delay(5000);
  
  Serial.begin(115200);
  Serial1.begin(115200);
  
  ESP.begin();
  Serial.print("SSID: ");Serial.println(ESP.getSsid());
  Serial.print("Access: ");Serial.println(ESP.getAccess());
  Serial.print("Mode: ");Serial.println(ESP.getMode());
  Serial.print("IPMode: ");Serial.println(ESP.getIpMode());
  Serial.print("Status: ");Serial.println(ESP.getStatus());
  Serial.print("AccessIP: ");Serial.println(ESP.getAccessIP());
  Serial.print("StationIP: ");Serial.println(ESP.getStationIP());
  Serial.print("Ready> ");
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
