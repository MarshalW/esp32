#include <WireGuard-ESP32.h>
#include <WiFi.h>
#include "time.h"

#include "config.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 8;
const int   daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA); // Optional
  WiFi.begin(SSID, PASS);
  Serial.println("\nConnecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

  // We configure the NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
  delay(1000);
}
