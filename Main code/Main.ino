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
#define LIGHT_SENSOR_PIN 5 // chân 5
int Lightvalue;
#define LED_PIN 15 // chân 15
int lightThreshold = 300; // Ngưỡng cường độ ánh sáng để bật LED
unsigned long ledOnTime = 2 * 60 * 60 * 1000; // Thời gian bật LED (2 giờ)

unsigned long ledTurnOnMillis; // Biến lưu thời điểm bật LED

// Water sensor
#define SIGNAL_PIN  35// Analog pin for reading sensor value
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
    pinMode(LED_PIN, OUTPUT);

    dht.begin();
}

void displayTime(void) {
DateTime now = rtc.now();

// Xóa màn hình
display.clearDisplay();

// Hiển thị ngày, tháng, năm và thời gian
display.setCursor(0,0);
display.print(now.year(), DEC);
display.print('/');
display.print(now.month(), DEC);
display.print('/');
display.print(now.day(), DEC);
display.print(" ");
display.print(now.hour(), DEC);
display.print(':');
display.print(now.minute(), DEC);
display.print(':');
display.print(now.second(), DEC);

// Hiển thị mặt trời hoặc mặt trăng và bóng đèn dựa vào giá trị cảm biến ánh sáng
if (Lightvalue == HIGH) { // Ban ngày
    // Vẽ mặt trời
    display.drawCircle(64, 16, 8, WHITE); // Vẽ một hình tròn tại vị trí (64,16) với bán kính 8 pixel
    // Vẽ bóng đèn tắt
    display.drawRect(120, 12, 8, 10, WHITE); // Vẽ phần thân của bóng đèn
    display.drawLine(124, 22, 124, 26, WHITE); // Vẽ phần chân của bóng đèn
} else { // Ban đêm
    // Vẽ mặt trăng
    display.drawCircle(64, 16, 8, WHITE); // Vẽ hình tròn lớn
    display.fillCircle(70, 16, 8, BLACK); // Vẽ hình tròn nhỏ để tạo hiệu ứng mặt trăng lưỡi liềm
    // Vẽ bóng đèn sáng
    display.drawRect(120, 12, 8, 10, WHITE); // Vẽ phần thân của bóng đèn
    display.drawLine(124, 22, 124, 26, WHITE); // Vẽ phần chân của bóng đèn
    display.drawLine(120, 14, 128, 14, WHITE); // Vẽ phần sáng của bóng đèn
}


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

    if (lightValue < lightThreshold && digitalRead(LED_PIN) == LOW) { // Nếu cường độ ánh sáng nhỏ hơn ngưỡng và LED đang tắt
        digitalWrite(LED_PIN, HIGH); // Bật LED
        ledTurnOnMillis = millis(); // Lưu lại thời điểm bật LED
    }

    if (digitalRead(LED_PIN) == HIGH && millis() - ledTurnOnMillis >= ledOnTime) { // Nếu LED đang bật và đã qua khoảng thời gian ledOnTime
        digitalWrite(LED_PIN, LOW); // Tắt LED
    }

// Cảm biến đất
    int value = analogRead(soilMoisturePin);
    value = map(value, 0, 4095, 0, 100);
    value = (value - 100) * -1;
    Blynk.virtualWrite(V8, value);
    Serial.print("Soil: ");
    Serial.println(value);

    if (value < 20) {
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
    lcd.print(value);
    lcd.print("%");
    
    // Hiển thị mặt trời hoặc mặt trăng và bóng đèn dựa vào giá trị cảm biến ánh sáng
    if (Lightvalue == HIGH) { // Ban ngày
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
    lcdText += "\nSoil Moisture: " + String(value) + "%";
    // Hiển thị mặt trời hoặc mặt trăng và bóng đèn dựa vào giá trị cảm biến ánh sáng
    if (Lightvalue == HIGH) { // Ban ngày
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
