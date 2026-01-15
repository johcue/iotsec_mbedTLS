#pragma once
#include <Arduino.h>
#include <WiFiClient.h>

extern "C" {
  #include <wolfssl/ssl.h>
}

class WolfSSLClient {
public:
  WolfSSLClient();

  bool setCredentials(const String& ca,
                      const String& cert,
                      const String& key);

  bool connect(const char* host, uint16_t port);

  void stop();

private:
  WiFiClient tcp;
  WOLFSSL_CTX* ctx = nullptr;
  WOLFSSL* ssl = nullptr;

  String caPem;
  String certPem;
  String keyPem;
};
