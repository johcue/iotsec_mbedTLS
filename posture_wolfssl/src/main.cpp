#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <wolfssl/ssl.h>
#include <vector>

/* ================= Binary CA ================= */
std::vector<uint8_t> caDer;

/* ================= Helpers ================= */
bool loadFileBin(const char* path, std::vector<uint8_t>& out) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;

  size_t n = f.size();
  if (n == 0) {
    f.close();
    return false;
  }

  out.resize(n);
  size_t r = f.read(out.data(), n);
  f.close();
  return r == n;
}

/* ================= Setup ================= */
void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println();
  Serial.println("=== wolfSSL TLS-only test (DER CA) ===");

  /* ---------- LittleFS ---------- */
  Serial.println("[SYS] Mounting LittleFS...");
  if (!LittleFS.begin(false)) {
    Serial.println("[FATAL] LittleFS mount failed");
    return;
  }

  Serial.println("[SYS] Loading CA (DER)...");
  if (!loadFileBin("ca.der", caDer)) {
    Serial.println("[FATAL] Missing /ca.der in LittleFS");
    return;
  }
  Serial.printf("[SYS] CA DER size: %u bytes\n", (unsigned)caDer.size());

  /* ---------- WiFi ---------- */
  Serial.println("[NET] Connecting WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[NET] WiFi connected");
  Serial.println(WiFi.localIP());

  /* ---------- TCP ---------- */
  Serial.println("[TEST] Starting TLS test...");
  WiFiClient tcp;

  Serial.println("[TLS] Opening TCP connection...");
  if (!tcp.connect(BROKER_HOST, BROKER_PORT)) {
    Serial.println("[TLS] TCP connect failed");
    return;
  }

  /* ---------- wolfSSL ---------- */
  Serial.println("[TLS] Initializing wolfSSL...");
  wolfSSL_Debugging_ON();
  wolfSSL_Init();

  Serial.println("[TLS] Creating wolfSSL context...");
  WOLFSSL_CTX* ctx =
      wolfSSL_CTX_new(wolfTLSv1_2_client_method());

  if (!ctx) {
    Serial.println("[TLS] CTX creation failed");
    return;
  }

  wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_PEER, NULL);

  /* ---------- Load CA (ASN.1) ---------- */
  Serial.println("[TLS] Loading CA (ASN.1/DER)...");
  int ret = wolfSSL_CTX_load_verify_buffer(
      ctx,
      caDer.data(),
      (int)caDer.size(),
      WOLFSSL_FILETYPE_ASN1);

  if (ret != WOLFSSL_SUCCESS) {
    Serial.printf("[TLS] CA DER load failed (ret=%d)\n", ret);
    wolfSSL_CTX_free(ctx);
    return;
  }

  Serial.println("[TLS] CA loaded OK (DER)");
  Serial.println();
  Serial.println("=== RESULT: CA LOAD SUCCESS ===");
}

/* ================= Loop ================= */
void loop() {
  delay(1000);
}
