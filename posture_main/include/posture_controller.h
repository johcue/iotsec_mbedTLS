#pragma once
#include <Arduino.h>

struct PostureReading {
  float pitch = 0.0f;
  float roll  = 0.0f;
  bool bad    = false;
};

bool posture_init();               // init IMU + motor pin
void posture_calibrate();          // baseline calibration
void posture_update(PostureReading& out);  // compute pitch/roll + bad/good
