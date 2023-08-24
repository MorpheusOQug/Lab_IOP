#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6GQGuLZ6D"
#define BLYNK_TEMPLATE_NAME "Lab1"
#define BLYNK_AUTH_TOKEN "ebkBKK1uZdPjdir_w-2zF64YXMT_r0hS"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


char ssid[] = "VJU Guest";
char pass[] = "Vjuguest@2022";

#define WATER_PUMP_1 26


boolean state = false;



void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);
  pinMode(WATER_PUMP_1, OUTPUT);
}


void loop() {
  Blynk.run();
}

BLYNK_WRITE(V1)
{
  if(state == false){
    state = true;
    digitalWrite(WATER_PUMP_1,HIGH);
    delay(1000);
  }
  else{
    state = false;
    digitalWrite(WATER_PUMP_1,LOW);

  }
}
