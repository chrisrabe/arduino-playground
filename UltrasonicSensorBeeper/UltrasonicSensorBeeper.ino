#include "SR04.h"

const int buzzerPin = 8;
const int trigPin = 4; // sensor input
const int echoPin = 5; // sensor output

SR04 sr04 = SR04(echoPin, trigPin);

const int MIN_DISTANCE_CM = 5;    // at or below this, buzzer stays on constantly
const int MAX_DISTANCE_CM = 100;  // at or beyond this, buzzer stays silent
const int BEEP_MS = 40;           // length of each beep
const int MIN_GAP_MS = 30;        // silence between beeps when very close
const int MAX_GAP_MS = 800;       // silence between beeps when far away

long a;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  a = sr04.Distance();
  Serial.print(a);
  Serial.println("cm");

  // Out of range (or no echo, which the library reports as 0): stay silent
  if (a <= 0 || a >= MAX_DISTANCE_CM) {
    digitalWrite(buzzerPin, LOW);
    delay(100);
    return;
  }

  // Very close: continuous tone
  if (a <= MIN_DISTANCE_CM) {
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    return;
  }

  // In between: the closer the object, the shorter the gap between beeps
  int gap = map(a, MIN_DISTANCE_CM, MAX_DISTANCE_CM, MIN_GAP_MS, MAX_GAP_MS);

  digitalWrite(buzzerPin, HIGH);
  delay(BEEP_MS);
  digitalWrite(buzzerPin, LOW);
  delay(gap);
}