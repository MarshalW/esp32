#include <WiFi.h>
#include <ESPping.h>

#include "config.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to the AP...");
  delay(1000);

  WiFi.mode(WIFI_STA); // Optional
  WiFi.begin(SSID, PASS);
  Serial.println("\nConnecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void loop() {

  IPAddress ip; 
  ip.fromString(PING_IP);
  
  bool success = Ping.ping(ip);

  if(!success){
    Serial.println("Ping failed");
    return;
  }
 
  Serial.println("Ping succesful.");
  
  delay(1000);
}