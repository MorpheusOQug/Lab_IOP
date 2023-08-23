#define PIROUT 5
void setup()
{
  Serial.begin(9600);
  pinMode(PIROUT, INPUT);
}
void loop()
{
  if (digitalRead(PIROUT)==HIGH)
  {
    Serial.println("Some body is here");
  }
  else
  {
    Serial.println("Nothing Detected by PIR Sensor");
  }
  delay(1000);
}
