#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "mbedtls/sha256.h"

static void print_hash(const char* path) {
  File f = LittleFS.open(path, "r");
  if (!f) {
    Serial.printf("[ERR] Missing %s\n", path);
    return;
  }

  mbedtls_sha256_context ctx;
  mbedtls_sha256_init(&ctx);
  mbedtls_sha256_starts(&ctx, 0);  // 0 = SHA-256 (not SHA-224)

  uint8_t buf[512];
  while (true) {
    int n = f.read(buf, sizeof(buf));
    if (n <= 0) break;
    mbedtls_sha256_update(&ctx, buf, n);
  }

  uint8_t out[32];
  mbedtls_sha256_finish(&ctx, out);
  mbedtls_sha256_free(&ctx);

  Serial.printf("[OK] %s sha256=", path);
  for (int i = 0; i < 32; i++) {
    Serial.printf("%02x", out[i]);
  }
  Serial.println();

  f.close();
}

void setup() {
  Serial.begin(9600);
  delay(500);

  Serial.println("\n=== ESP32 PROVISIONING CHECK ===");

  if (!LittleFS.begin(false)) {
    Serial.println("[FATAL] LittleFS mount failed");
    Serial.println("Run: pio run -t uploadfs");
    while (true) delay(1000);
  }

  print_hash("/ca.crt");
  print_hash("/client.crt");
  print_hash("/client.key");
  print_hash("/mqtt.json");

  Serial.println("Provisioning OK. Flash main firmware next.");
}

void loop() {
  delay(1000);
}
