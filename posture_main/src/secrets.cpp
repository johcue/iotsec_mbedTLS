#include "secrets.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

static bool readFileToString(const char* path, String& out) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;
  out = f.readString();
  f.close();
  return out.length() > 0;
}

bool secrets_begin() {
  return LittleFS.begin(false);
}

bool secrets_load_tls(String& ca, String& cert, String& key) {
  return readFileToString("/ca.crt", ca) &&
         readFileToString("/client.crt", cert) &&
         readFileToString("/client.key", key);
}

bool secrets_load_mqtt(MqttConfig& cfg) {
  File f = LittleFS.open("/mqtt.json", "r");
  if (!f) return false;

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return false;

  cfg.host  = (const char*)(doc["host"]  | "");
  cfg.port  = (int)(doc["port"] | 8883);
  cfg.user  = (const char*)(doc["user"]  | "");
  cfg.pass  = (const char*)(doc["pass"]  | "");
  cfg.topic = (const char*)(doc["topic"] | "");

  if (cfg.host.length() == 0 || cfg.user.length() == 0 || cfg.pass.length() == 0 || cfg.topic.length() == 0) {
    return false;
  }
  return true;
}
