#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7);

/* Pin Layout:
 *  LCD
 *    SDA to A4 on arduino
 *    SCL to A5 on arduino
 *    VCC to 5v on arduino
 *    GND to GND on arduino
*/

void setup() {
  lcd.begin (16,2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("LCD Test");
  lcd.setCursor(0,1);
  lcd.print("LCD Test");
}

void loop() {
}
