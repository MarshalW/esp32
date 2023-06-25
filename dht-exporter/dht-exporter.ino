#include <WireGuard-ESP32.h>
#include <WiFi.h>
#include <ESPping.h>
#include <DHT.h>
#include "time.h"
#include <WebServer.h>

#include "config.h"

static WireGuard wg;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 8;
const int   daylightOffset_sec = 0;

IPAddress local_ip;

float temperature;
float humidity;

unsigned long lastSend;

DHT dht(DATAPIN, DHTTYPE);

WebServer server(EXPORTER_PORT);

void metrics() {  
  String message;
  message += F("dhtexp_temperature "); 
  message += String(temperature); 
  message += F("\ndhtexp_humidity ");
  message += String(humidity); 
  
  server.send(200, "text/plain", message);
}

void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Get all data at /metrics"));
    });
    server.on(F("/metrics"), HTTP_GET, metrics);
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  dht.begin();
  
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

  restServerRouting();
  server.begin();
  Serial.println("HTTP server started");

  // We configure the NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  local_ip.fromString(LOCAL_IP);

  wg.begin(
    local_ip,
    PRIVATE_KEY,
    ENDPOINT_ADDRESS,
    PEER_PUBLIC_KEY,
    ENDPOINT_PORT);
}

void getData(){
  temperature = dht.readTemperature(); // Gets the values of the temperature
  humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }
}

void loop() {
  struct tm timeinfo;

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getData();
    lastSend = millis();
  }

  server.handleClient();
  delay(200);
}
