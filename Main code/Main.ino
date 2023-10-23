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

// Tạo ký tự tùy chỉnh cho bóng đèn
byte bulbChar[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000,
  0b01110,
};

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
char ssid[] = "VJU-FTH1";
char pass[] = "0987654321";

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
#define pumpPin 26 // Chân 33
int pumpStatus = LOW;

// Soil Moisture
#define soilMoisturePin 33 // Chân 33

// Light Sensor
#define LIGHT_SENSOR_PIN 34 // chân 5
#define LED1_PIN 13 // chân 15
#define LED2_PIN 14 // chân 15
#define LED3_PIN 2 // chân 15
int lightThreshold1 = 80; // Ngưỡng cường độ ánh sáng để tắt cả ba đèn LED
int lightThreshold2 = 40; // Ngưỡng cường độ ánh sáng để bật đèn LED 1 và đèn LED 2
int lightThreshold3 = 10; // Ngưỡng cường độ ánh sáng để bật cả ba đèn LED

unsigned long ledOnTime = 2 * 60 * 60 * 1000; // Thời gian bật LED (2 giờ)

unsigned long ledTurnOnMillis; // Biến lưu thời điểm bật LED

// Water sensor
#define SIGNAL_PIN  35// Analog pin for reading sensor value
float waterIndex = 0.0;

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
    lcd.createChar(2, bulbChar); // Tạo ký tự tùy chỉnh cho bóng đèn

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

    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin, pumpStatus);

    pinMode(soilMoisturePin, INPUT);

    pinMode(LIGHT_SENSOR_PIN, INPUT);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    dht.begin();
}

void displayTime(void) {
DateTime now = rtc.now();

// Xóa màn hình
display.clearDisplay();
display.setTextSize(1);             // Normal 1:1 pixel scale
display.setCursor(0,0);
display.setTextColor(SSD1306_WHITE);        // Draw white text
display.print("  - IOP Robotech - ");
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
    int lightValue = analogRead(LIGHT_SENSOR_PIN); // Đọc giá trị từ cảm biến ánh sáng
    lightValue = map(lightValue, 0, 4095, 0, 100);
    lightValue = (lightValue - 100) * -1;
    Blynk.virtualWrite(V2, lightValue);
    Serial.print("Light: ");
    Serial.println(lightValue);

    if (lightValue > lightThreshold1) { // Nếu cường độ ánh sáng lớn hơn ngưỡng 1
        digitalWrite(LED1_PIN, HIGH); // Tắt LED 1
        digitalWrite(LED2_PIN, HIGH); // Tắt LED 2
        digitalWrite(LED3_PIN, HIGH); // Tắt LED 3
    } else if (lightValue >= lightThreshold2 && lightValue <= lightThreshold1) { // Nếu cường độ ánh sáng trong khoảng ngưỡng 2 và ngưỡng 1
        digitalWrite(LED1_PIN, LOW); // Bật LED 1
        digitalWrite(LED2_PIN, HIGH); // Tắt LED 2
        digitalWrite(LED3_PIN, HIGH); // Tắt LED 3
    } else if (lightValue >= lightThreshold3 && lightValue < lightThreshold2) { // Nếu cường độ ánh sáng trong khoảng ngưỡng 3 và ngưỡng 2
        digitalWrite(LED1_PIN, LOW); // Bật LED 1
        digitalWrite(LED2_PIN, LOW); // Bật LED 2
        digitalWrite(LED3_PIN, HIGH); // Tắt LED 3
    } else { // Nếu cường độ ánh sáng nhỏ hơn ngưỡng 3
        digitalWrite(LED1_PIN, LOW); // Bật LED 1
        digitalWrite(LED2_PIN, LOW); // Bật LED 2
        digitalWrite(LED3_PIN, LOW); // Bật LED 3
    }
    if (millis() - ledTurnOnMillis >= ledOnTime) { // Nếu LED đang bật và đã qua khoảng thời gian ledOnTime
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW); // Tắt LED
    }

// Cảm biến đất
    int Svalue = analogRead(soilMoisturePin);
    Svalue = map(Svalue, 0, 4095, 0, 100);
    Svalue = (Svalue - 100) * -1;
    Blynk.virtualWrite(V8, Svalue);
    Serial.print("Soil: ");
    Serial.println(Svalue);

    if (Svalue < 20) {
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
    waterIndex = map(rawValue, 0, 4095, 0, 100);
    waterIndex = (waterIndex - 100) * -1;
    Blynk.virtualWrite(V3, waterIndex);
    Serial.print("\nWater Index: ");
    Serial.println(waterIndex);

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
    lcd.print(Svalue);
    lcd.print("%");
    
    // Hiển thị mặt trời hoặc mặt trăng và bóng đèn dựa vào giá trị cảm biến ánh sáng
    if (lightValue == HIGH) { // Ban ngày
        lcd.setCursor(13, 1); // Đặt vị trí mới cho mặt trời
        lcd.write(byte(0)); // Hiển thị mặt trời
        lcd.setCursor(14, 1); // Đặt vị trí mới cho bóng đèn
        lcd.print(" "); // Hiển thị bóng đèn tắt (không hiển thị gì cả)
    } else { // Ban đêm
        lcd.setCursor(13, 1); // Đặt vị trí mới cho mặt trăng
        lcd.write(byte(1)); // Hiển thị mặt trăng
        lcd.setCursor(14, 1); // Đặt vị trí mới cho bóng đèn
        lcd.write(byte(2)); // Hiển thị bóng đèn sáng
    }
    
// Send LCD text to Blynk app
    String lcdText = "Temp: " + String(temperature) + "C\n" + "Humidity: " + String(humidity) + "%";
    lcdText += "\nSoil Moisture: " + String(Svalue) + "%";
    // Hiển thị mặt trời hoặc mặt trăng và bóng đèn dựa vào giá trị cảm biến ánh sáng
    if (lightValue == HIGH) { // Ban ngày
        lcdText = "Day: Sun, Bulb Off";
    } else { // Ban đêm
        lcdText = "Night: Moon, Bulb On";
    }
    Blynk.virtualWrite(V9, lcdText);

    
    delay(1000);
}

// Hàm này sẽ được gọi mỗi khi chúng ta ghi vào chân ảo V8 từ ứng dụng Blynk
BLYNK_WRITE(V7) {
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

BLYNK_WRITE(V6) {
    int pinData = param.asInt(); // Nhận dữ liệu dưới dạng số nguyên

    ledOnTime = pinData * 60 * 60 * 1000; // Cập nhật thời gian bật LED theo giá trị nhận được từ Blynk app
}
