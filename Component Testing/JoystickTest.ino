#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7); //A4 to SDA, A5 to SCL
//JOYSTICK: xAxis to A0, yAxis to A1, Switch to D8


/* Pin Layout:
 *  JOYSTICK:
 *    X to A0 on Arduino
 *    SW to D8 on Arduino
 *    VCC to 5V on Arduino
 *    GND to GND on Arduino
*/


void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

}

void loop() {
  Serial.println(digitalRead(8));   //this value (0 or 1) will indicate whether the joystick is clicked
  Serial.println(analogRead(0));    //this value will indicate where the joystick is pointing on the x axis
}
