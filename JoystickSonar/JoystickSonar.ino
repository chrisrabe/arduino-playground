#include <Servo.h>
#include "SR04.h"

Servo myservo;

// Servo
const int servoPin = 12;

// Joystick
const int j_SwitchPin = 2;
const int j_XPin = 0;
const int j_YPin = 1;

// Sonar
const int trigPin = 4;
const int echoPin = 5;

// Led + Buzzer
const int ledBuzzerPin = 8;

const int LEFT_THRESHOLD  = 10lp0 
const int RIGHT_THRESHOLD = 1013;

// Servo movement
const int STEP_SIZE  = 5;
const unsigned long STEP_DELAY = 15;   // ms between servo updates
int servoAngle = 90;

SR04 sr04 = SR04(echoPin, trigPin);

const int MIN_DISTANCE_CM = 5;
const int MAX_DISTANCE_CM = 100;
const unsigned long BEEP_MS = 40;
const unsigned long MIN_GAP_MS = 30;
const unsigned long MAX_GAP_MS = 800;
const unsigned long SONAR_INTERVAL = 100;  // ms between distance readings

// Timing state
unsigned long lastServoTime = 0;
unsigned long lastSonarTime = 0;
unsigned long lastBeepChange = 0;

// Shared state between the sonar reader and the beeper
long distanceCm = 0;
bool buzzerOn = false;

void setup() {
  pinMode(ledBuzzerPin, OUTPUT);
  myservo.attach(servoPin);
  myservo.write(servoAngle);
  pinMode(j_SwitchPin, INPUT_PULLUP);
  Serial.begin(9600);
}

// Task 1: read the sensor periodically (only stores the result)
void updateDistance(unsigned long now) {
  if (now - lastSonarTime < SONAR_INTERVAL) return;
  lastSonarTime = now;

  distanceCm = sr04.Distance();
  Serial.print(distanceCm);
  Serial.println("cm");
}

// Task 2: drive the buzzer as a little state machine
void updateBuzzer(unsigned long now) {
  // Out of range / no echo: silent
  if (distanceCm <= 0 || distanceCm >= MAX_DISTANCE_CM) {
    digitalWrite(ledBuzzerPin, LOW);
    buzzerOn = false;
    return;
  }

  // Very close: continuous tone
  if (distanceCm <= MIN_DISTANCE_CM) {
    digitalWrite(ledBuzzerPin, HIGH);
    buzzerOn = true;
    return;
  }

  // In between: toggle between beep and gap based on elapsed time
  unsigned long gap = map(distanceCm, MIN_DISTANCE_CM, MAX_DISTANCE_CM,
                          MIN_GAP_MS, MAX_GAP_MS);
  unsigned long wait = buzzerOn ? BEEP_MS : gap;

  if (now - lastBeepChange >= wait) {
    buzzerOn = !buzzerOn;
    digitalWrite(ledBuzzerPin, buzzerOn ? HIGH : LOW);
    lastBeepChange = now;
  }
}

// Task 3: step the servo at a fixed rate
void updateServo(unsigned long now) {
  if (now - lastServoTime < STEP_DELAY) return;
  lastServoTime = now;

  const int yValue = analogRead(j_YPin);

  if (yValue <= LEFT_THRESHOLD) {
    servoAngle -= STEP_SIZE;
  } else if (yValue >= RIGHT_THRESHOLD) {
    servoAngle += STEP_SIZE;
  }

  servoAngle = constrain(servoAngle, 0, 180);
  myservo.write(servoAngle);
}

void loop() {
  unsigned long now = millis();
  updateDistance(now);
  updateBuzzer(now);
  updateServo(now);
}