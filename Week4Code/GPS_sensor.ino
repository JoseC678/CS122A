//Code for extracting GPGLL information
//GPGLL , Longitude, Latitude
//Got half of the code from
//https://docs.arduino.cc/built-in-examples/communication/SerialEvent
//Which showss how to automatically read in incoming information from serial data until the end
//of the line to test the next line if is mathced with $GPGLL

String RxInput = "";
boolean InputReady = false;
String check = "\$GPGLL";


void setup() {
  Serial.begin(9600);
  RxInput.reserve(200);
}

void loop() {
  // put your main code here, to run repeatedly:

  //A string of GPGLL has the format of
  //$GPGLL,3348.01325,N,11712.87620,W,173203.00,A,D*7C

  if (InputReady) {
    String temp = RxInput.substring(0, 6); //get the ID
    if (temp == check) { //Check if its $GPGLL
      String Lattitude = RxInput.substring(7, 17); //The longitude starts at 17
      String Longitude = RxInput.substring(20, 31); //The longitude starts at 20
    
    
    Serial.print("LAT:" );
    Serial.println((Lattitude.toFloat())+32.005);

    Serial.print("LON:" );
    Serial.println((Longitude.toFloat())+8.587);
    }

    RxInput = "";
    InputReady = false;


  }
}

//From the link in the begening.
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    RxInput += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      InputReady = true;
    }
  }
}
