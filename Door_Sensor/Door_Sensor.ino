
const byte MagnetSens = 8;
const byte DoorOpen = 9;


void setup() {
  //start serial connection
  Serial.begin(115200);
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(MagnetSens, INPUT_PULLUP);
  pinMode(DoorOpen, OUTPUT);

}

void loop() {
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(MagnetSens);
  //print out the value of the pushbutton
  Serial.println(sensorVal);

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  if (sensorVal == HIGH) {
    digitalWrite(DoorOpen, LOW);
  } else {
    digitalWrite(DoorOpen, HIGH);
  }
  delay(1000);
}
