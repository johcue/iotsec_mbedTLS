# Secure Posture Monitoring System and TLS Engine Evaluation for IoT

This repository presents an **end-to-end secure IoT application** and a **controlled comparison of lightweight TLS engines** for embedded systems.

The project combines:
- A wearable **posture monitoring system** based on ESP32 and IMU sensors
- An experimental evaluation of **TLS engines** securing MQTT communication

---

## Secure Posture Monitoring System (MQTT over mTLS)

The system monitors body orientation and provides real-time feedback when poor posture is detected.

- **ESP32** acquires motion data from an **MPU6500 IMU**
- Pitch and roll are estimated using a complementary filter
- A calibrated baseline defines correct posture
- A vibration motor provides haptic feedback
- Posture data is published as JSON over **MQTT**
- A **Node-RED** dashboard visualizes posture data

### Secure Communication

Communication is secured using **MQTT over TLS with mutual authentication (mTLS)**.

- **Mosquitto** broker running in Docker with TLS enabled
- Client authentication enforced (no anonymous access)
- ESP32 credentials stored in **LittleFS**, not in firmware
- Node-RED uses its own client certificate
- A private **CA** authenticates all entities

TLS is applied strictly at the transport layer.

---

## TLS Engine Evaluation (mbedTLS vs wolfSSL)

The project evaluates lightweight TLS engines commonly used in IoT:

- **mbedTLS** via `WiFiClientSecure`
- **wolfSSL** as an alternative TLS engine

### Comparison Scope

- Identical system architecture
- Unchanged MQTT logic and application behavior
- Identical certificate hierarchy and mTLS configuration
- **Only the TLS engine differs**

### Metrics

- TLS handshake time
- Heap memory usage during secure connection
- Developer usability (integration and configuration effort)

wolfSSL is used exclusively as a TLS backend, enabling drop-in replacement under identical constraints.

---

## Repository Structure
```text
├── posture_main/        # Secure posture monitoring application
├── posture_provision/   # TLS credential provisioning (LittleFS)
├── posture_wolfssl/     # wolfSSL-based TLS evaluation client
└── README.md
```

---

## Demo
▶ [Secure Posture Monitoring Demo (MQTT over mTLS)](https://drive.google.com/file/d/1Y2OlpshMnMuMwvWeSMBaiK7FP02cnzj8/view)

---

## Key Properties

- End-to-end encrypted MQTT
- Mutual authentication (mTLS)
- No anonymous connections
- Credentials stored outside firmware binaries
- TLS engine isolated for fair evaluation

---

## Intended Use

- Secure IoT system design
- Embedded TLS evaluation
- MQTT security experimentation
- Academic and educational purposes
