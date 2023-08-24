#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2); 

void setup()
{
  lcd.init();                    
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print(" Dientunhattung");
  lcd.setCursor(0,1);
  lcd.print("Huynh Nhat Tung ");
}

void loop()
{
}