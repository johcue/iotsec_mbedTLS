#include "posture_controller.h"
#include <Wire.h>
#include <MPU6500_WE.h>

/* ================= IMU ================= */
static MPU6500_WE myMPU(0x68);

/* ================= HARDWARE ================= */
static const int motorPin = 5;

/* ================= POSTURE ================= */
static float baselinePitch = 0.0f;
static float baselineRoll  = 0.0f;

static const float THRESHOLD_PITCH = 10.0f;
static const float THRESHOLD_ROLL  = 10.0f;
static const float alpha = 0.95f;

static float pitch = 0.0f;
static float roll  = 0.0f;

/* ================= TIMING ================= */
static unsigned long prevTime = 0;

/* ================= CALIBRATION ================= */
static const int CALIBRATION_TIME_MS  = 2000;
static const int CALIBRATION_DELAY_MS = 50;

/* ================= SYSTEM STATE ================= */
static bool filterWarmupDone = false;
static const int FILTER_WARMUP_SAMPLES = 50;
static int warmupCounter = 0;

bool posture_init() {
  Wire.begin();

  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  Serial.println("Initializing MPU6500...");
  if (!myMPU.init()) {
    Serial.println("MPU6500 not responding!");
    return false;
  }

  myMPU.autoOffsets();
  myMPU.setAccRange(MPU9250_ACC_RANGE_4G);
  myMPU.setGyrRange(MPU9250_GYRO_RANGE_500);
  myMPU.enableGyrDLPF();
  myMPU.setGyrDLPF(MPU9250_DLPF_6);
  myMPU.setSampleRateDivider(5);

  prevTime = millis();
  filterWarmupDone = false;
  warmupCounter = 0;
  pitch = 0.0f;
  roll  = 0.0f;

  return true;
}

void posture_calibrate() {
  Serial.println("Calibrating posture in 2 secs...");
  delay(2000);
  Serial.println("Sit straight and be still!");

  float sumPitch = 0.0f;
  float sumRoll  = 0.0f;
  int samples = CALIBRATION_TIME_MS / CALIBRATION_DELAY_MS;

  for (int i = 0; i < samples; i++) {
    xyzFloat acc = myMPU.getAccRawValues();

    float accPitch = atan2(acc.y, sqrt(acc.x * acc.x + acc.z * acc.z)) * 180.0f / PI;
    float accRoll  = atan2(-acc.x, acc.z) * 180.0f / PI;

    sumPitch += accPitch;
    sumRoll  += accRoll;

    delay(CALIBRATION_DELAY_MS);
  }

  baselinePitch = sumPitch / samples;
  baselineRoll  = sumRoll  / samples;

  Serial.print("Calibration done. Baseline Pitch: ");
  Serial.print(baselinePitch, 2);
  Serial.print(" | Baseline Roll: ");
  Serial.println(baselineRoll, 2);
}

void posture_update(PostureReading& out) {
  xyzFloat acc = myMPU.getAccRawValues();
  xyzFloat gyr = myMPU.getGyrValues();

  unsigned long now = millis();
  float dt = (now - prevTime) / 1000.0f;
  prevTime = now;

  float accPitch = atan2(acc.y, sqrt(acc.x * acc.x + acc.z * acc.z)) * 180.0f / PI;
  float accRoll  = atan2(-acc.x, acc.z) * 180.0f / PI;

  pitch = alpha * (pitch + gyr.x * dt) + (1.0f - alpha) * accPitch;
  roll  = alpha * (roll  + gyr.y * dt) + (1.0f - alpha) * accRoll;

  // warm-up to stabilize filter
  if (!filterWarmupDone) {
    warmupCounter++;
    if (warmupCounter >= FILTER_WARMUP_SAMPLES) {
      filterWarmupDone = true;
      Serial.println("Filter stabilized.");
    }
    digitalWrite(motorPin, LOW);
    out.pitch = pitch;
    out.roll = roll;
    out.bad = false;
    return;
  }

  float deltaPitch = pitch - baselinePitch;
  float deltaRoll  = roll  - baselineRoll;

  bool badPosture = (fabs(deltaPitch) > THRESHOLD_PITCH ||
                     fabs(deltaRoll)  > THRESHOLD_ROLL);

  // motor control
  digitalWrite(motorPin, badPosture ? HIGH : LOW);

  out.pitch = pitch;
  out.roll  = roll;
  out.bad   = badPosture;
}
