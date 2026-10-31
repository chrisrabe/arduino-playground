#include <Servo.h>
Servo myservo;

void setup() {
  myservo.attach(9);
}

void loop() {
  myservo.write(180);  // spin one direction
  delay(2000);
  myservo.write(90);   // stop
  delay(1000);
  myservo.write(0);    // spin the other direction
  delay(2000);
  myservo.write(90);   // stop
  delay(1000);
}