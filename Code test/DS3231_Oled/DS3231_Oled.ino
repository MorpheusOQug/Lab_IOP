#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6GQGuLZ6D"
#define BLYNK_TEMPLATE_NAME "Lab1"
#define BLYNK_AUTH_TOKEN "ebkBKK1uZdPjdir_w-2zF64YXMT_r0hS"


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "VJU Guest";
char pass[] = "Vjuguest@2022";


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "RTClib.h"  //  RTC library
#include <Wire.h>   // i2c libary
#include <Adafruit_GFX.h>   //  graphic library
#include <Adafruit_SSD1306.h>   // oled library

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  //  initialize display

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
RTC_DS3231 rtc;  //  rtc class

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).


void setup () {

  Serial.begin(9600);

  delay(3000); // wait for console opening

  if (! rtc.begin()) {      //   initialize RTC
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // If the RTC have lost power it will sets the RTC to the date & time this sketch was compiled in the following line
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // set time from computer time
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // If you need to set the time of the uncomment line 34 or 37
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
 }

// Connect to Wi-Fi and Blynk server
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}

void loop () {

Blynk.run();
displayTime();   // printing time function for oled

}

void displayTime(void) {
DateTime now = rtc.now();

display.clearDisplay();
display.setTextSize(1);             // Normal 1:1 pixel scale
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
