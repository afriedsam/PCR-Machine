/* Pin Layout:
 *  Thermocouple
 *    DO to D3 on arduino
 *    CS to D4 on arduino
 *    CLK to D5 on arduino
 *    VCC to 5V on arduino
 *    GND to GND on arduino
*/

#include <Adafruit_MAX31855.h>
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
double temp = thermocouple.readCelsius();

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(temp);
  delay(500);
}
