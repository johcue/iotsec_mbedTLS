#include "mqtt_tls.h"

#if defined(USE_WOLFSSL)
  #include "WolfSSLClient.h"
  static WolfSSLClient secureClient;
#else
  #include <WiFiClientSecure.h>
  static WiFiClientSecure secureClient;
#endif

static PubSubClient client(secureClient);

void mqtt_tls_setup(const MqttConfig& cfg,
                    const String& ca,
                    const String& clientCert,
                    const String& clientKey) {

#if defined(USE_WOLFSSL)

  secureClient.setCACert(ca);
  secureClient.setCertificate(clientCert);
  secureClient.setPrivateKey(clientKey);

#else

  secureClient.setCACert(ca.c_str());
  secureClient.setCertificate(clientCert.c_str());
  secureClient.setPrivateKey(clientKey.c_str());

#endif

  client.setServer(cfg.host.c_str(), cfg.port);
}

bool mqtt_tls_connect_blocking(const MqttConfig& cfg, const char* clientId) {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT (mTLS)...");

    bool ok = client.connect(
        clientId,
        cfg.user.c_str(),
        cfg.pass.c_str());

    if (ok) {
      Serial.println(" connected!");
      return true;
    }

    Serial.print(" failed, rc=");
    Serial.println(client.state());
    delay(3000);
  }
  return true;
}

PubSubClient& mqtt_client() {
  return client;
}
