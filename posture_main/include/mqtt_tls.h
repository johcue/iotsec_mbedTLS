#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include "secrets.h"

void mqtt_tls_setup(const MqttConfig& cfg,
                    const String& ca,
                    const String& clientCert,
                    const String& clientKey);

bool mqtt_tls_connect_blocking(const MqttConfig& cfg, const char* clientId);

PubSubClient& mqtt_client();
