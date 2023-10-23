//Sg90 arduino max 180 degrees
#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(8);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  myservo.write(0);      // sets the servo position to 0 degrees
  delay(1000);           // waits for a second
  myservo.write(90);    // sets the servo position to 180 degrees
  delay(1000);           // waits for a second
}