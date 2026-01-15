#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Client.h>
#include <wolfssl/ssl.h>

class WolfSSLClient : public Client {
public:
    WolfSSLClient();
    ~WolfSSLClient();

    /* TLS material (PEM in memory) */
    void setCACert(const String& ca);
    void setCertificate(const String& cert);
    void setPrivateKey(const String& key);

    /* Client interface */
    int connect(IPAddress ip, uint16_t port) override;
    int connect(const char* host, uint16_t port) override;

    size_t write(uint8_t byte) override;
    size_t write(const uint8_t* buf, size_t size) override;

    int read() override;
    int read(uint8_t* buf, size_t size) override;

    int available() override;
    int peek() override;
    void flush() override;
    void stop() override;
    uint8_t connected() override;
    operator bool() override;

private:
    bool startTLS();

    WiFiClient tcp_;
    WOLFSSL_CTX* ctx_;
    WOLFSSL* ssl_;

    String ca_;
    String cert_;
    String key_;
};
