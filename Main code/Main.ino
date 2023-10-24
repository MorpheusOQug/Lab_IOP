// Libraries
#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Blynk settings
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6GQGuLZ6D"
#define BLYNK_TEMPLATE_NAME "Lab1"
#define BLYNK_AUTH_TOKEN "ebkBKK1uZdPjdir_w-2zF64YXMT_r0hS"

// WiFi settings
char ssid[] = "VJU-FTH1";
char pass[] = "0987654321";

// Constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define NUMFLAKES 10
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

// Objects
RTC_DS3231 rtc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

// Pins
#define pumpPin 26
#define soilMoisturePin 34
#define LIGHT_SENSOR_PIN 33
#define LED1_PIN 13
#define LED2_PIN 14
#define LED3_PIN 2
#define SIGNAL_PIN 35

// Light settings
int lightThreshold1 = 80;
int lightThreshold2 = 40;
int lightThreshold3 = 10;
unsigned long ledOnTime = 2 * 60 * 60 * 1000;
unsigned long ledTurnOnMillis;

// Water sensor
float waterIndex = 0.0;

// Time settings
int startHour = 6;
int startMinute = 0;
int endHour = 6;
int endMinute = 10;

// Mode and status
bool manualMode = false;
bool isPumpOn = false;

void setup() {
    Serial.begin(9600);
    delay(3000);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    dht.begin();

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin, pumpStatus);

    pinMode(soilMoisturePin, INPUT);

    pinMode(LIGHT_SENSOR_PIN, INPUT);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    dht.begin();
}

void displayTime() {
    DateTime now = rtc.now();

    // Display time on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);
    display.print("  - IOP Robotech - ");
    display.setCursor(15, 15);
    display.print(now.year(), DEC);
    display.print('/');
    display.print(now.month(), DEC);
    display.print('/');
    display.print(now.day(), DEC);

    display.print(" ");

    display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    display.println();
    display.println();
    display.setTextSize(2);
    display.print(" ");
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();

    // Blynk virtual write for time
    String currentTime = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String currentDate = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
    Blynk.virtualWrite(V4, currentTime);
    Blynk.virtualWrite(V5, currentDate);

    display.display();
    delay(1000);
}

void loop() {
    Blynk.run();
    displayTime();
    DateTime now = rtc.now();

    // Light sensor
    int lightValue = analogRead(LIGHT_SENSOR_PIN);
    lightValue = map(lightValue, 0, 4095, 0, 100);
    lightValue = (lightValue - 100) * -1;
    Blynk.virtualWrite(V2, lightValue);
    Serial.print("Light: ");
    Serial.println(lightValue);

    if (lightValue > lightThreshold1) {
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(LED3_PIN, HIGH);
    } else if (lightValue >= lightThreshold2 && lightValue <= lightThreshold1) {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(LED3_PIN, HIGH);
    } else if (lightValue >= lightThreshold3 && lightValue < lightThreshold2) {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, HIGH);
    } else {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
    }

    if (millis() - ledTurnOnMillis >= ledOnTime) {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
    }

    // Soil moisture sensor
    int Svalue = analogRead(soilMoisturePin);
    Svalue = map(Svalue, 0, 4095, 0, 100);
    Svalue = (Svalue - 100) * -1;
    Blynk.virtualWrite(V8, Svalue);
    Serial.print("Soil: ");
    Serial.println(Svalue);

    if (Svalue < 35 && !isPumpOn) {
        digitalWrite(pumpPin, LOW);
        pumpStatus = LOW;
        isPumpOn = true;
        delay(10000);
    } else if (Svalue >= 55 && isPumpOn) {
        digitalWrite(pumpPin, HIGH);
        pumpStatus = HIGH;
        isPumpOn = false;
    }

    // Temperature and humidity
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);

    Serial.print("\n");
    Serial.print("Humidity: " + String(humidity) + "%");
    Serial.print("\t");
    Serial.print("Temperature:" + String(temperature) + "C");
    delay(1000);

    // Rainwater sensor
    int rawValue = analogRead(SIGNAL_PIN);
    waterIndex = map(rawValue, 0, 4095, 0, 100);
    waterIndex = (waterIndex - 100) * -1;
    Blynk.virtualWrite(V3, waterIndex);
    Serial.print("\nWater Index: ");
    Serial.println(waterIndex);

    // Send LCD text to Blynk app
    String lcdText = "Temp: " + String(temperature) + "C\n" + "Humidity: " + String(humidity) + "%";
    lcdText += "\nSoil Moisture: " + String(Svalue) + "%";

    if (lightValue == HIGH) {
        lcdText = "Day: Sun, Bulb Off";
    } else {
        lcdText = "Night: Moon, Bulb On";
    }

    Blynk.virtualWrite(V9, lcdText);

    delay(1000);
}

// Blynk callback for manual mode
BLYNK_WRITE(V7) {
    int pinData = param.asInt();

    if (pinData == 1) {
        digitalWrite(pumpPin, HIGH);
        pumpStatus = HIGH;
        manualMode = true;
    } else {
        digitalWrite(pumpPin, LOW);
        pumpStatus = LOW;
        manualMode = false;
    }
}

// Blynk callback for LED on time
BLYNK_WRITE(V6) {
    int pinData = param.asInt();
    ledOnTime = pinData * 60 * 60 * 1000;
}
