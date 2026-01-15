#include <Arduino.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "mqtt_tls.h"

void wifi_connect();

#include "posture_controller.h"

static MqttConfig cfg;  // global so loop() can publish to cfg.topic

static unsigned long lastPublish = 0;
static const unsigned long publishIntervalMs = 500;

void setup() {
  Serial.begin(9600);
  delay(300);

  if (!secrets_begin()) {
    Serial.println("[FATAL] LittleFS mount failed. Run posture_provision uploadfs first.");
    while (true) delay(1000);
  }

  String ca, clientCert, clientKey;

  if (!secrets_load_tls(ca, clientCert, clientKey)) {
    Serial.println("[FATAL] Missing TLS files in LittleFS: /ca.crt /client.crt /client.key");
    while (true) delay(1000);
  }
  if (!secrets_load_mqtt(cfg)) {
    Serial.println("[FATAL] Missing/invalid /mqtt.json in LittleFS");
    while (true) delay(1000);
  }

  if (!posture_init()) {
    Serial.println("[FATAL] posture_init failed");
    while (true) delay(1000);
  }

  wifi_connect();
  mqtt_tls_setup(cfg, ca, clientCert, clientKey);
  mqtt_tls_connect_blocking(cfg, "esp32-posture-001");

  posture_calibrate();

  Serial.println("System ready. Monitoring posture...");
}

void loop() {
  mqtt_client().loop();

  PostureReading r;
  posture_update(r);

  unsigned long now = millis();
  if (now - lastPublish >= publishIntervalMs) {
    JsonDocument doc;
    doc["pitch"]  = r.pitch;
    doc["roll"]   = r.roll;
    doc["status"] = r.bad ? "bad" : "good";

    char buffer[256];
    serializeJson(doc, buffer);

    mqtt_client().publish(cfg.topic.c_str(), buffer);

    lastPublish = now;
  }

  delay(50);
}
