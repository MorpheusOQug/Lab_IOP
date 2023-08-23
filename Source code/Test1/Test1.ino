#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6GQGuLZ6D"
#define BLYNK_TEMPLATE_NAME "Lab1"
#define BLYNK_AUTH_TOKEN "ebkBKK1uZdPjdir_w-2zF64YXMT_r0hS"
#define timeSeconds 10




// Your WiFi credentials
char ssid[] = "VJU Guest";
char pass[] = "Vjuguest@2022";


// Blynk connection
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
// Set GPIOs for LED and PIR Motion Sensor
const int led = 26;
const int motionSensor = 27;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;

boolean startTimer = false;
boolean motion = false;

// Checks if motion was detected, sets LED HIGH and starts a timer
void IRAM_ATTR detectsMovement() {
  digitalWrite(led, HIGH);
  startTimer = true;
  lastTrigger = millis();
}


// Your Blynk token

void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);
  
  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Set LED to LOW
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  

  // Blynk connection
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);
  
}

void loop() {
  // Current time
  now = millis();
  if((digitalRead(led) == HIGH) && (motion == false)) {
    Serial.println("MOTION DETECTED!!!");
    motion = true;

  }
  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    Serial.println("Motion stopped...");
    digitalWrite(led, LOW);
    startTimer = false;
    motion = false;
  }
}