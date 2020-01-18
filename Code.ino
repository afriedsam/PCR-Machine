/* Pin Layout:
 *  JOYSTICK:
 *    X to A0
 *    SW to D8
 *  LCD
 *    SDA to A4
 *    SCL to A5
 *  Thermocouple
 *    DO to D3
 *    CS to D4
 *    CLK to D5
 *  Relay
 *    Pos to D13
*/

#include <Adafruit_MAX31855.h>
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2,1,0,4,5,6,7);

long duration;
int cycles = 1;
int currentCycle = 1;
long startTime;
long timeEllapsed;
long endTime;
long remainder;
double annealingTemp = 50.00;
void heaterOn() {digitalWrite(13, HIGH);}
void fanOn() {digitalWrite(13, LOW);}

void getCycles() {
  lcd.setCursor(5,0);
  lcd.print("CYCLES:");
  lcd.setCursor(8,1);
  lcd.print(cycles);
  if (analogRead(0) < 100 && cycles >= 2) {
    cycles -= 1;
    delay(400);
  }
  if (analogRead(0) > 900) {
    cycles += 1;
    delay(400);
  }
  if (digitalRead(8) == 1) {
    getCycles();
  }
}

void getAnnealTemp() {
  lcd.setCursor(1,0);
  lcd.print("ANNEALING TEMP:");
  lcd.setCursor(5,1);
  lcd.print(annealingTemp);
  if (analogRead(0) < 100 && annealingTemp >= 49) {
    annealingTemp -= 0.25;
    delay(400);
  }
  if (analogRead(0) > 900) {
    annealingTemp += 0.25;
    delay(400);
  }
  if (digitalRead(8) == 1) {
    getAnnealTemp();
  }
}

void enter() {
  lcd.setCursor(2,0);
  lcd.print("FRIEDSAM PCR");
  lcd.setCursor(1,1);
  lcd.print("CLICK TO BEGIN");
  if (digitalRead(8) == 1) {
    enter();
  }
}

void getInput() {
  getCycles();
  lcd.clear();
  delay(500);
  getAnnealTemp();
  lcd.clear();
  delay(500);
  enter();
  lcd.clear();
  delay(500);
}

void prepDenaturation() {
  heaterOn();
  double temp = thermocouple.readCelsius();
  delay(500);
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("HEATING");
  lcd.setCursor(2,1);
  lcd.print("Temp: ");
  if (isnan(temp) == false) {
    lcd.setCursor(8,1);
    lcd.print(temp);
  }
  if (temp<95 || isnan(temp)) {
    prepDenaturation();   //if it is less than 95 degrees keep heating
  }
}
void denature() {
  double temp = thermocouple.readCelsius();
  if (temp <= 94.75) {
     heaterOn();
   }
   if (temp >= 95.25) {
      fanOn();
   }
   timeEllapsed = millis();
   endTime = startTime + duration;
   remainder = endTime - timeEllapsed;
   delay(500);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("PHASE: DENATURE");
   lcd.setCursor(0,1);
   lcd.print("CYCLE: ");
   lcd.setCursor(8,1);
   lcd.print(currentCycle);
   lcd.setCursor(10,1);
   lcd.print(temp);
   lcd.setCursor(15,1);
   lcd.print("C");
   if (timeEllapsed < endTime) {
    denature();
   }
}

void prepAnnealing() {
  fanOn();
  double temp = thermocouple.readCelsius();
  delay(500);
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("COOLING");
  lcd.setCursor(2,1);
  lcd.print("Temp: " + String(temp) + "C");
  if (temp >= annealingTemp + 0.25 || isnan(temp)) {
    prepAnnealing();
  }
}

void anneal() {
  double temp = thermocouple.readCelsius();
  if (temp <= annealingTemp - 0.25) {
     heaterOn();
   }
   if (temp >= annealingTemp + 0.25) {
      fanOn();
   }
   timeEllapsed = millis();
   endTime = startTime + duration;
   remainder = endTime - timeEllapsed;
   delay(500);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("PHASE: ANNEAL");
   lcd.setCursor(0,1);
   lcd.print("CYCLE: ");
   lcd.setCursor(8,1);
   lcd.print(currentCycle);
   lcd.setCursor(10,1);
   lcd.print(temp);
   lcd.setCursor(15,1);
   lcd.print("C");
   if (timeEllapsed < endTime) {
    anneal();
   }
}

void prepExtension() {    //heat back up
  heaterOn();
  double temp = thermocouple.readCelsius();
  delay(500);
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("HEATING");
  lcd.setCursor(2,1);
  lcd.print("Temp: " + String(temp) + "C");
  if (temp < 72 || isnan(temp)) {
    prepExtension();
  }
}

void extend() {
  double temp = thermocouple.readCelsius();
  if (temp <= 71.75) {
     heaterOn();
   }
   if (temp >= 72.25) {
      fanOn();
   }
   timeEllapsed = millis();
   endTime = startTime + duration;
   remainder = endTime - timeEllapsed;
   delay(500);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("PHASE: EXTEND");
   lcd.setCursor(2,1);
   lcd.print("CYCLE: " + currentCycle);
   lcd.setCursor(0,1);
   lcd.print("CYCLE: ");
   lcd.setCursor(8,1);
   lcd.print(currentCycle);
   lcd.setCursor(10,1);
   lcd.print(temp);
   lcd.setCursor(15,1);
   lcd.print("C");
   if (timeEllapsed < endTime) {
    extend();
   }
}

void PCR() {
  for (int i = 0; i <= cycles-1; i++) {
    prepDenaturation();
    lcd.clear();
    duration = 30000;
    startTime = millis();
    denature();
    lcd.clear();
    prepAnnealing();
    lcd.clear();
    duration = 30000;
    startTime = millis();
    anneal();
    lcd.clear();
    prepExtension();
    lcd.clear();
    duration = 60000;
    startTime = millis();
    extend();
    lcd.clear();
    currentCycle += 1;
  }
  fanOn();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("FRIEDSAM PCR");
  lcd.setCursor(0,1);
  lcd.print("PHASE: COMPLETE");
}

void setup() {
  pinMode(13, OUTPUT);
  lcd.begin (16,2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  //Serial.begin(9600);
  delay(10000);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
  getInput();
  PCR();
}

void loop() {
}
