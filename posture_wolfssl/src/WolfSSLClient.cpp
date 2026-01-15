#include "WolfSSLClient.h"

WolfSSLClient::WolfSSLClient() {
  wolfSSL_Init();
}

bool WolfSSLClient::setCredentials(const String& ca,
                                   const String& cert,
                                   const String& key) {
  caPem = ca;
  certPem = cert;
  keyPem = key;
  return true;
}

bool WolfSSLClient::connect(const char* host, uint16_t port) {
  Serial.println("[TLS] Opening TCP connection...");
  if (!tcp.connect(host, port)) {
    Serial.println("[TLS] TCP connect failed");
    return false;
  }

  Serial.println("[TLS] Creating wolfSSL context...");
  ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
  if (!ctx) {
    Serial.println("[TLS] wolfSSL_CTX_new failed");
    return false;
  }

  Serial.println("[TLS] Loading CA...");
  if (wolfSSL_CTX_load_verify_buffer(
        ctx,
        (const unsigned char*)caPem.c_str(),
        caPem.length(),
        WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
    Serial.println("[TLS] CA load failed");
    return false;
  }

  Serial.println("[TLS] Loading client certificate...");
  if (wolfSSL_CTX_use_certificate_buffer(
        ctx,
        (const unsigned char*)certPem.c_str(),
        certPem.length(),
        WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
    Serial.println("[TLS] Client cert load failed");
    return false;
  }

  Serial.println("[TLS] Loading private key...");
  if (wolfSSL_CTX_use_PrivateKey_buffer(
        ctx,
        (const unsigned char*)keyPem.c_str(),
        keyPem.length(),
        WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
    Serial.println("[TLS] Private key load failed");
    return false;
  }

  wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);

  ssl = wolfSSL_new(ctx);
  if (!ssl) {
    Serial.println("[TLS] wolfSSL_new failed");
    return false;
  }

  wolfSSL_set_fd(ssl, tcp.fd());

  Serial.println("[TLS] Starting TLS handshake...");
  int ret = wolfSSL_connect(ssl);
  if (ret != WOLFSSL_SUCCESS) {
    int err = wolfSSL_get_error(ssl, ret);
    Serial.printf("[TLS] Handshake FAILED (err=%d)\n", err);
    return false;
  }

  Serial.println("[TLS] Handshake SUCCESS");
  Serial.printf("[TLS] Cipher: %s\n", wolfSSL_get_cipher(ssl));
  Serial.printf("[TLS] Version: %s\n", wolfSSL_get_version(ssl));

  return true;
}

void WolfSSLClient::stop() {
  if (ssl) {
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    ssl = nullptr;
  }
  if (ctx) {
    wolfSSL_CTX_free(ctx);
    ctx = nullptr;
  }
  tcp.stop();
}
