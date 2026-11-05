#include <Wire.h>
const int MPU_addr = 0x68;  // I2C address of the MPU-6050

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int redPin = 5;
int greenPin = 4;
int bluePin = 3;

// Calibration: the readings board gives when flat and still
const int OFFSET_X = -700;
const int OFFSET_Y = -50;

// Thresholds (raw counts; 16384 = 1 g, 131 = 1 degree/second)
const int TILT_LIMIT = 1500;   // about 5 degrees
const int MOTION_LIMIT = 1500; // about 11 degrees/second

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void getMpuReadings() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);                      // start at ACCEL_XOUT_H
  Wire.endTransmission(false);           // keep the connection open
  Wire.requestFrom(MPU_addr, 14, true);  // read 14 bytes

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

void setColor(bool r, bool g, bool b) {
  digitalWrite(redPin, r ? HIGH : LOW);
  digitalWrite(greenPin, g ? HIGH : LOW);
  digitalWrite(bluePin, b ? HIGH : LOW);
}

void loop() {
  getMpuReadings();

  // Remove the flat-board bias
  int x = AcX - OFFSET_X;
  int y = AcY - OFFSET_Y;

  bool moving = abs(GyX) > MOTION_LIMIT ||
                abs(GyY) > MOTION_LIMIT ||
                abs(GyZ) > MOTION_LIMIT;
  bool tilted = abs(x) > TILT_LIMIT || abs(y) > TILT_LIMIT;

  if (moving) {
    setColor(false, false, true);   // blue
  } else if (tilted) {
    setColor(true, false, false);   // red
  } else {
    setColor(false, true, false);   // green
  }

  delay(50);
}