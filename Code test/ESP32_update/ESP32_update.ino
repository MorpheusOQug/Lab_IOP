#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6GQGuLZ6D"
#define BLYNK_TEMPLATE_NAME "Lab1"
#define BLYNK_AUTH_TOKEN "ebkBKK1uZdPjdir_w-2zF64YXMT_r0hS"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Tạo ký tự tùy chỉnh cho mặt trời
byte sunChar[8] = {
  0b00000,
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101,
  0b00000,
};

// Tạo ký tự tùy chỉnh cho mặt trăng
byte moonChar[8] = {
  0b00000,
  0b01100,
  0b01110,
  0b01111,
  0b01111,
  0b01110,
  0b01100,
};

// Get wifi
char ssid[] = "VJU Guest";
char pass[] = "Vjuguest@2022";

//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

RTC_DS3231 rtc;

// PUMP
#define pumpPin 33 // Chân 33
int pumpStatus = LOW;

// Soil Moisture
#define soilMoisturePin 18 // Chân 18

// Light Sensor
#define Sensor 5 // chân 5
int Lightvalue;
#define Ledpin 15 // chân 15

// Water sensor
#define SIGNAL_PIN  32// Analog pin for reading sensor value
float waterIndex = 0.0;
// Define the minimum and maximum values that your sensor can read
const int MIN_SENSOR_VALUE = 0;   // Replace with your actual minimum value
const int MAX_SENSOR_VALUE = 1023; // Replace with your actual maximum value

#define DHTPIN  4 // chân 
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int startHour = 6;
int startMinute = 0;
int endHour = 6;
int endMinute = 10;

bool manualMode = false; // Biến mới để theo dõi chế độ hoạt động

void setup() {
    Serial.begin(9600);
    delay(3000);

    lcd.begin(16,2);
    lcd.clear();
    lcd.print("Starting...");

    lcd.createChar(0, sunChar);
    lcd.createChar(1, moonChar);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC,  0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;) ;
    }

    dht.begin();

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    pinMode(Ledpin, OUTPUT);
    digitalWrite(Ledpin, LOW);

    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin, pumpStatus);

    pinMode(soilMoisturePin, INPUT);

    dht.begin();
}

void displayTime(void) {
DateTime now = rtc.now();

display.clearDisplay();
display.setTextSize(1);             // Normal : pixel scale
display.setCursor(0,0);
display.setTextColor(SSD1306_WHITE);        // Draw white text
display.print("void loop Robotech");
display.setCursor(15,15);            // Start at top-left corner
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

// Hẹn giờ bơm nước
    if (!manualMode && (now.hour() > startHour || (now.hour() == startHour && now.minute() >= startMinute)) &&
        (now.hour() < endHour || (now.hour() == endHour && now.minute() <= endMinute))) {
        digitalWrite(pumpPin, HIGH);
        pumpStatus = HIGH;
    } else if (!manualMode) {
        digitalWrite(pumpPin, LOW);
        pumpStatus = LOW;
    }
// Hàm cảm biến ánh sáng
    Lightvalue = digitalRead(Sensor);
    Blynk.virtualWrite(V6, Lightvalue);

    if (Lightvalue == HIGH)
        digitalWrite(Ledpin, HIGH);
    else
        digitalWrite(Ledpin, LOW);
// Cảm biến đất
    int pValue = analogRead(soilMoisturePin);
    int soilMoistureValue = map(pValue, 0, 4095, 0, 100);
    soilMoistureValue = (soilMoistureValue - 100) * -1;
    Blynk.virtualWrite(V8, soilMoistureValue);
    Serial.print("Soil: ");
    Serial.println(soilMoistureValue);

    if (soilMoistureValue < 20) {
        digitalWrite(pumpPin, HIGH);
        pumpStatus = HIGH;
        delay(5000);
        digitalWrite(pumpPin, LOW);
        pumpStatus = LOW;
    }
// Đọc Temp & Hum
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
    delay(2000);

// Rain water sensor
    int rawValue = analogRead(SIGNAL_PIN); // Read the analog value from the sensor

    waterIndex = map(rawValue, MIN_SENSOR_VALUE, MAX_SENSOR_VALUE, 0, 100) / 100.0;

    Serial.print("\nWater Index: ");
    Serial.println(waterIndex);

    Blynk.virtualWrite(V3, waterIndex * 100); // Send water index (percentage) to Blynk

// Hàm ghi LCD 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C Hum: ");
    lcd.print(humidity);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Soil-M: ");
    lcd.print(soilMoistureValue);
    lcd.print("%");

    // Hiển thị hình chiếc quạt chuyển động (ví dụ: sử dụng ký tự '>' để biểu diễn quạt)
    static int fanPos = 0;
    const char fanAnim[] = ">-<";
    
    // Hiển thị mặt trời hoặc mặt trăng dựa vào giá trị cảm biến ánh sáng
    if (Lightvalue == HIGH) {
        lcd.setCursor(13, 1); // Đặt vị trí mới cho mặt trời/mặt trăng
        lcd.write(byte(0)); // Hiển thị mặt trời
        lcd.setCursor(14, 1); // Đặt vị trí mới cho quạt
        lcd.write(fanAnim[fanPos]); // Hiển thị quạt
    } else {
        lcd.setCursor(13, 1); // Đặt vị trí mới cho mặt trời/mặt trăng
        lcd.write(byte(1)); // Hiển thị mặt trăng
        lcd.setCursor(14, 1); // Đặt vị trí mới cho quạt
        lcd.write(fanAnim[fanPos]); // Hiển thị quạt
    }
    
    fanPos = (fanPos + 1) % strlen(fanAnim); // Cập nhật vị trí quạt cho lần lặp tiếp theo

// Send LCD text to Blynk app
    String lcdText = "Temp: " + String(temperature) + "C\n" + "Humidity: " + String(humidity) + "%";
    lcdText += "\nSoil Moisture: " + String(soilMoistureValue) + "%";
    lcdText += "\nFan: " + String(fanAnim[fanPos]);
    lcdText += (Lightvalue == HIGH) ? "\nSun" : "\nMoon";
    Blynk.virtualWrite(V9, lcdText);

    
    delay(1000);
}

// Hàm này sẽ được gọi mỗi khi chúng ta ghi vào chân ảo V8 từ ứng dụng Blynk
BLYNK_WRITE(V8) {
    int pinData = param.asInt(); // Nhận dữ liệu dưới dạng số nguyên

    if (pinData == 1) {
        digitalWrite(pumpPin, HIGH); // Bật bơm
        pumpStatus = HIGH;
        manualMode = true; // Chuyển sang chế độ thủ công
    } else {
        digitalWrite(pumpPin, LOW); // Tắt bơm
        pumpStatus = LOW;
        manualMode = false; // Chuyển về chế độ tự động
    }
}