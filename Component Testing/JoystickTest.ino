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
