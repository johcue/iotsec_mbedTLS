#pragma once
#include <Arduino.h>

struct MqttConfig {
  String host;
  int port = 8883;
  String user;
  String pass;
  String topic;
};

bool secrets_begin();
bool secrets_load_tls(String& ca, String& cert, String& key);
bool secrets_load_mqtt(MqttConfig& cfg);
