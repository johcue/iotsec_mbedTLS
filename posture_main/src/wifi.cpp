#include <Arduino.h>
#include <WiFi.h>
#include "wifi_config.h"

void wifi_connect() {
  Serial.print("\nConnecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}
