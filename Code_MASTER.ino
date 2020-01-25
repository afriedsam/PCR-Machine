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
long firstDenaturationDuration;
long fDDSeconds = 0;
long fDDMins = 02;
long lastExtensionDuration;
long fEDSeconds = 0;
long fEDMins = 05;
long endTime;
long remainder;
double annealingTemp = 50.00;

void heaterOn() {digitalWrite(13, HIGH);}
void fanOn() {digitalWrite(13, LOW);}

void getCycles() {
  lcd.setCursor(5,0);
  lcd.print("CYCLES:");
  lcd.setCursor(7,1);
  lcd.print(cycles);
  if (analogRead(0) < 100 && cycles >= 2) {
    cycles -= 1;
    delay(200);
  }
  if (analogRead(0) > 900) {
    cycles += 1;
    delay(200);
  }
  if (digitalRead(8) == 1) {
    getCycles();
  }
}
  
void getFirstDenaturationDuration() {
  lcd.setCursor(0,0);
  lcd.print("1st Denaturation");
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  lcd.setCursor(6,1);
  lcd.print(fDDMins);
  lcd.setCursor(7,1);
  lcd.print(":");
  if (fDDSeconds > 59) {
    fDDMins += 1;
    fDDSeconds = 0;
    delay(250);
  }
  if (fDDSeconds == 0 && analogRead(0) < 100) {
    fDDMins -= 1;
    fDDSeconds = 59;
    delay(250);
  }
  if (fDDSeconds >= 9) {
    lcd.setCursor(8,1);
    lcd.print(fDDSeconds);
  }
  if (fDDSeconds < 10 && fDDSeconds > 0) {
    lcd.setCursor(8,1);
    lcd.print("0");
    lcd.setCursor(9,1);
    lcd.print(fDDSeconds);
  }
  if (fDDSeconds == 0) {
    lcd.setCursor(8,1);
    lcd.print("00");
  }
  firstDenaturationDuration = ((fDDMins * 60000) + (fDDSeconds * 1000));
  if (analogRead(0) < 100 && firstDenaturationDuration>30000) {
    fDDSeconds -= 1;
    delay(200);
  }
  if (analogRead(0) > 900 && fDDMins < 8) {
    fDDSeconds += 1;
    delay(200);
  }
  firstDenaturationDuration = (fDDMins * 60000) + (fDDSeconds * 1000);
  if (digitalRead(8) == 1) {
    getFirstDenaturationDuration();
  }
}

void getAnnealTemp() {
  lcd.setCursor(1,0);
  lcd.print("ANNEALING TEMP:");
  lcd.setCursor(5,1);
  lcd.print(annealingTemp);
  if (analogRead(0) < 100 && annealingTemp >= 49) {
    annealingTemp -= 0.25;
    delay(200);
  }
  if (analogRead(0) > 900) {
    annealingTemp += 0.25;
    delay(200);
  }
  if (digitalRead(8) == 1) {
    getAnnealTemp();
  }
}

void getLastExtensionDuration() {
  lcd.setCursor(1,0);
  lcd.print("Last Extension");
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  lcd.setCursor(6,1);
  lcd.print(fEDMins);
  lcd.setCursor(7,1);
  lcd.print(":");
  if (fEDSeconds > 59) {
    fEDMins += 1;
    fEDSeconds = 0;
    delay(250);
  }
  if (fEDSeconds == 0 && analogRead(0) < 100) {
    fEDMins -= 1;
    fEDSeconds = 59;
    delay(250);
  }
  if (fEDSeconds >= 9) {
    lcd.setCursor(8,1);
    lcd.print(fEDSeconds);
  }
  if (fEDSeconds < 10 && fEDSeconds > 0) {
    lcd.setCursor(8,1);
    lcd.print("0");
    lcd.setCursor(9,1);
    lcd.print(fEDSeconds);
  }
  if (fEDSeconds == 0) {
    lcd.setCursor(8,1);
    lcd.print("00");
  }
  lastExtensionDuration = ((fEDMins * 60000) + (fEDSeconds * 1000));
  if (analogRead(0) < 100 && lastExtensionDuration>60000) {
    fEDSeconds -= 1;
    delay(200);
  }
  if (analogRead(0) > 900 && fEDMins < 8) {
    fEDSeconds += 1;
    delay(200);
  }
  lastExtensionDuration = (fEDMins * 60000) + (fEDSeconds * 1000);
  if (digitalRead(8) == 1) {
    getLastExtensionDuration();
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
  getFirstDenaturationDuration();
  lcd.clear();
  delay(500);
  getAnnealTemp();
  lcd.clear();
  delay(500);
  getLastExtensionDuration();
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
   lcd.print("CYCLE:");
   lcd.setCursor(7,1);
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
  lcd.print("Temp: ");
  if (isnan(temp) == false) {
    lcd.setCursor(8,1);
    lcd.print(temp);
  }
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
   lcd.print("CYCLE:");
   lcd.setCursor(7,1);
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
  lcd.print("Temp: ");
  if (isnan(temp) == false) {
    lcd.setCursor(8,1);
    lcd.print(temp);
  }
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
   lcd.setCursor(0,1);
   lcd.print("CYCLE:");
   lcd.setCursor(7,1);
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
    if (currentCycle == 1) {duration = firstDenaturationDuration;}
    else {duration = 30000;}
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
    if (cycles - currentCycle == 0) {duration = lastExtensionDuration;}
    else {duration = 60000;}
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
  Serial.begin(9600);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
  getInput();
  PCR();
}

void loop() {
}
