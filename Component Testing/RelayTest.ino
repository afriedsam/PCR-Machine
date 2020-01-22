/* IF YOUR RELAY WORKS THIS CODE SHOULD ACTIVATE AND DEACTIVATE THE SWITCH EVERY 5 SECONDS 
 *  
 *  Pin Layout:
 *  Relay
 *    Pos to D13 on arduino
 *    Neg to GND on arduino
*/


void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH):
  delay(5000);
  digitalWrite(13, LOW);
  delay(5000);
}
