#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6GQGuLZ6D"
#define BLYNK_TEMPLATE_NAME "Lab1"
#define BLYNK_AUTH_TOKEN "ebkBKK1uZdPjdir_w-2zF64YXMT_r0hS"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "VJU Guest";
char pass[] = "Vjuguest@2022";

int Sensor = 5; // Chân kết nối cảm biến ánh sáng
int Value;
int Ledpin = 13; // Chân điều khiển đèn LED

void setup() 
{
  Serial.begin(9600);
  pinMode(Ledpin, OUTPUT); 
  digitalWrite(Ledpin, LOW); // Tắt đèn LED ban đầu
  pinMode(Sensor, INPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);
}

void loop() 
{
  Value = digitalRead(Sensor);
  Serial.print("Value read from sensor: ");
  Serial.println(Value);

  // Send sensor value to virtual pin V1
  Blynk.virtualWrite(V6, Value);

  if (Value == HIGH) // Sử dụng toán tử so sánh "==" để kiểm tra tín hiệu HIGH
    digitalWrite(Ledpin, HIGH); // Bật đèn LED khi tín hiệu là HIGH
  else
    digitalWrite(Ledpin, LOW); // Tắt đèn LED khi tín hiệu là LOW
    
  delay(1000);
}
