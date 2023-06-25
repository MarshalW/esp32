#include <WireGuard-ESP32.h>
#include <WiFi.h>
#include <ESPping.h>
#include "time.h"

#include "config.h"

static WireGuard wg;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 8;
const int   daylightOffset_sec = 0;

IPAddress local_ip;
IPAddress peer_ip;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }
  
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

  // We configure the NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  local_ip.fromString(LOCAL_IP);
  peer_ip.fromString(PEER_IP);

  wg.begin(
    local_ip,
    PRIVATE_KEY,
    ENDPOINT_ADDRESS,
    PEER_PUBLIC_KEY,
    ENDPOINT_PORT);
}

void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
 
  
  bool success = Ping.ping(peer_ip);

  if(!success){
    Serial.println("Ping failed");
  }else{
    Serial.println("Ping succesful.");
  }
  
  delay(1000);
}
