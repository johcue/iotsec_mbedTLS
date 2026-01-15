#include "WolfSSLClient.h"
#include <wolfssl/wolfcrypt/error-crypt.h>

WolfSSLClient::WolfSSLClient()
: ctx_(nullptr), ssl_(nullptr) {}

WolfSSLClient::~WolfSSLClient() {
    stop();
}

/* ================= TLS setters ================= */

void WolfSSLClient::setCACert(const String& ca) {
    ca_ = ca;
}

void WolfSSLClient::setCertificate(const String& cert) {
    cert_ = cert;
}

void WolfSSLClient::setPrivateKey(const String& key) {
    key_ = key;
}

/* ================= Client API ================= */

int WolfSSLClient::connect(IPAddress ip, uint16_t port) {
    if (!tcp_.connect(ip, port)) return 0;
    return startTLS() ? 1 : 0;
}

int WolfSSLClient::connect(const char* host, uint16_t port) {
    if (!tcp_.connect(host, port)) return 0;
    return startTLS() ? 1 : 0;
}

bool WolfSSLClient::startTLS() {
    wolfSSL_Init();

    ctx_ = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (!ctx_) {
        Serial.println("[wolfSSL] CTX creation failed");
        return false;
    }

    wolfSSL_CTX_set_verify(ctx_, WOLFSSL_VERIFY_PEER, NULL);

    if (wolfSSL_CTX_load_verify_buffer(
            ctx_,
            (const unsigned char*)ca_.c_str(),
            ca_.length(),
            WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        Serial.println("[wolfSSL] CA load failed");
        return false;
    }

    if (wolfSSL_CTX_use_certificate_buffer(
            ctx_,
            (const unsigned char*)cert_.c_str(),
            cert_.length(),
            WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        Serial.println("[wolfSSL] client cert load failed");
        return false;
    }

    if (wolfSSL_CTX_use_PrivateKey_buffer(
            ctx_,
            (const unsigned char*)key_.c_str(),
            key_.length(),
            WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        Serial.println("[wolfSSL] client key load failed");
        return false;
    }

    ssl_ = wolfSSL_new(ctx_);
    if (!ssl_) {
        Serial.println("[wolfSSL] SSL object creation failed");
        return false;
    }

    wolfSSL_set_fd(ssl_, tcp_.fd());

    int ret = wolfSSL_connect(ssl_);
    if (ret != WOLFSSL_SUCCESS) {
        int err = wolfSSL_get_error(ssl_, ret);
        Serial.printf("[wolfSSL] TLS handshake failed (%d)\n", err);
        return false;
    }

    Serial.println("[wolfSSL] TLS handshake OK");
    return true;
}

/* ================= Stream methods ================= */

size_t WolfSSLClient::write(uint8_t byte) {
    return write(&byte, 1);
}

size_t WolfSSLClient::write(const uint8_t* buf, size_t size) {
    if (!ssl_) return 0;
    int ret = wolfSSL_write(ssl_, buf, size);
    return ret > 0 ? ret : 0;
}

int WolfSSLClient::read() {
    uint8_t b;
    int r = read(&b, 1);
    return (r == 1) ? b : -1;
}

int WolfSSLClient::read(uint8_t* buf, size_t size) {
    if (!ssl_) return -1;
    return wolfSSL_read(ssl_, buf, size);
}

int WolfSSLClient::available() {
    if (!ssl_) return 0;
    return wolfSSL_pending(ssl_);
}

int WolfSSLClient::peek() {
    return -1;
}

void WolfSSLClient::flush() {
    tcp_.flush();
}

void WolfSSLClient::stop() {
    if (ssl_) {
        wolfSSL_shutdown(ssl_);
        wolfSSL_free(ssl_);
        ssl_ = nullptr;
    }
    if (ctx_) {
        wolfSSL_CTX_free(ctx_);
        ctx_ = nullptr;
    }
    tcp_.stop();
}

uint8_t WolfSSLClient::connected() {
    return tcp_.connected();
}

WolfSSLClient::operator bool() {
    return connected();
}
