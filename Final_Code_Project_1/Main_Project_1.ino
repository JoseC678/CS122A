#include <Wire.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

typedef struct _task{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int(*TickFct)(int);
}task;


const unsigned short numTasks = 4;
task tasks[numTasks]; 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;//From
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);// https://www.arduino.cc/en/Tutorial/LibraryExamples/HelloWorld




//TickFct_HBEAT_SM1
int TickFct_HBEAT_SM1();
enum SM1_States{SM1_Start, SM1_Wait} SM1_State;
int BPM = 0;

//TickFct_GPS_SM2
int TckFct_GPS_SM2();
enum SM2_States{SM2_Start, SM2_Wait}SM2_State;
String RxInput = "";
String Lattitude = "";
String Longitude = "";
boolean InputReady = false;
String check = "\$GPGLL";
float Lat_1_Position = 0;
float Long_1_Position = 0;
float Lat_2_Position = 0;
float Long_2_Position = 0;




//TickFct_Speed
int TickFct_Speed();
enum SM3_States{SM3_Start, SM3_Wait}SM3_State;
float Pos1_Long = 0;
float Pos1_Lat = 0;
float Pos2_Long = 0;
float Pos2_Lat = 0;
float MPH = 0;

//TickFct_Emergency button
int TickFct_Emergency();
enum SM4_States{SM4_Start, SM4_Wait}SM4_State;
const int buttonPin = 6;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
int buttonState = 0;  






int TickFct_HBEAT_SM1(int SM1_State){
  switch (SM1_State){
    case SM1_Start:
      SM1_State = SM1_Wait;
      break;
    case SM1_Wait:

      SM1_State = SM1_Wait;
      
      break;
    default:
      SM1_State = SM1_Wait;
      break;
  }


  switch (SM1_State){
    case SM1_Start:
      break;
    case SM1_Wait:
      //Wire.requestFrom(0xA0, 1); 
      Wire.requestFrom(0x50, 1);    // request 1 bytes from slave device Lets the 
                                          //Device know to send info
      while(Wire.available()) {          // slave may send less than requested
          BPM = Wire.read();   // receive heart rate value (a byte)
          Serial.print("BPM: ");
          Serial.println(BPM);         // print heart rate value
          
          //Print The BMP State Machine 1 to the LCD screen
          print_BPS_Value();
      }
      break;
    default:
      break;
  }

  return SM1_State;
}


int TickFct_GPS_SM2(int SM2_State){
  switch (SM2_State){
    case SM2_Start:
      SM2_State = SM2_Wait;
      break;
    case SM2_Wait:
      SM2_State = SM2_Wait;
      break;
    default:
      SM2_State = SM2_Wait;
      break;
  }


  switch (SM2_State){
    case SM2_Start:
      break;
    case SM2_Wait:
      //A string of GPGLL has the format of
      //$GPGLL,3348.01325,N,11712.87620,W,173203.00,A,D*7C
      if (InputReady) {
        String temp = RxInput.substring(0, 6); //get the ID
        if (temp == check) { //Check if its $GPGLL
          Lattitude = RxInput.substring(7, 17); //The longitude starts at 17
          Longitude = RxInput.substring(20, 31); //The longitude starts at 20
        
        
        Serial.print("LAT:" );
        Serial.println(((Lattitude.toFloat())+32.005)/100);
    
        Serial.print("LON:" );
        Serial.println(((Longitude.toFloat())+8.587)/100);
        }
    
        RxInput = "";
        InputReady = false;
    
    
      }
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
      break;
    default:
      break;
  }

  return SM2_State;
}



int TickFct_Speed(int SM3_State){
  switch (SM3_State){
    case SM3_Start:
      SM3_State = SM3_Wait;
      break;
    case SM3_Wait:
      //Testing with pre-set coordinates
      //float temp = 3600/(1600/((Distance_Calc(33.80134, 117.21310,33.80203, 117.21311))/5));

      float temp = 3600/(1600/((Distance_Calc(Lat_1_Position/100, Long_1_Position/100 , Lat_2_Position/100, Long_2_Position/100))/5));
      //Print the MPH
      lcd.setCursor(0, 1);
      //lcd.print("KPH:");
      lcd.print(temp);
      
      Lat_2_Position = Lat_1_Position;
      Long_2_Position = Long_1_Position;
      Lat_1_Position =  Lattitude.toFloat()+32.005;
      Long_1_Position = Longitude.toFloat()+8.587;

      //update the position
      //Serial.println(temp);
      //Serial.println("---------------");
      //Serial.println(Lat_2_Position);
      //Serial.println(Long_2_Position);
      //Serial.println(Lat_1_Position);
      //Serial.println(Long_1_Position);
      //Serial.println("---------------");

      lcd.setCursor(6, 1);
      lcd.print(" ");

      //Print The GPS sensor State Machine 2 Longituide and latitude 
      print_GPS_Value();

      MPH = temp; //Assign miles per value to check if it reached the limit.
      
      break;
    default:
      SM3_State = SM3_Wait;
      break;
  }


  switch (SM3_State){
    case SM3_Start:
      break;
    case SM3_Wait:
      break;
    default:
      break;
  }
  return SM3_State;
}


int TickFct_Emergency(int SM4_State){
  switch (SM4_State){
    case SM4_Start:
      SM4_State = SM4_Wait;
      break;
    case SM4_Wait:
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // Show the state of pushbutton on serial monitor
  Serial.println(buttonState);
      if ((buttonState == HIGH) && BPM > 30) {
      // turn LED on:
      digitalWrite(ledPin, HIGH);
      } else {
        // turn LED off:
        digitalWrite(ledPin, LOW);
      }
      break;
    default:
      SM4_State = SM4_Wait;
      break;
  }


  switch (SM4_State){
    case SM4_Start:
      break;
    case SM4_Wait:
      break;
    default:
      break;
  }
  return SM4_State;
}



void setup() {
    //Set up the Values and period
    unsigned char i=0;
    tasks[i].state= SM1_Start;
    tasks[i].period=1000;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_HBEAT_SM1;
    i++;
    tasks[i].state= SM2_Start;
    tasks[i].period=0;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_GPS_SM2;
    i++;
    tasks[i].state= SM3_Start;
    tasks[i].period=5000;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_Speed;

    i++;
    tasks[i].state= SM4_Start;
    tasks[i].period=200;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_Emergency;

    // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
    
    Serial.begin(9600);
    Wire.begin();
    lcd.begin(16, 2);
}



void loop() {
  unsigned short i;
  for (i = 0; i < numTasks; ++i) {
    if ((millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis();
      }
  }
}


void print_BPS_Value(){
      SM3_State = SM3_Wait;
      lcd.setCursor(0, 0);
      lcd.print("BPM:");
      char myStg[10];
      sprintf(myStg, "%d", BPM);
      if(BPM == 0){
        lcd.setCursor(0, 0);
        lcd.print("BPM:");
        lcd.setCursor(4, 0);
        lcd.print("0  ");
      }else{
        lcd.setCursor(4, 0);
        lcd.print(myStg);
      }
}

void print_GPS_Value(){
        float LAT;
      float LONG;
      String myStg2 = "";
      String myStg3 = "";
      //Assign Values
      LAT = Lattitude.toFloat()+32.005;
      LONG = Longitude.toFloat()+8.587;
      myStg2 = LAT;
      myStg3 = LONG;
      //sprintf(myStg2, "%d", Lattitude.toFloat()+32.005);
      //sprintf(myStg3, "%d", Longitude.toFloat()+8.587);
      lcd.setCursor(7, 0);
      lcd.print(myStg2);
      lcd.setCursor(7, 1);
      lcd.print(myStg3);
}

//https://github.com/mikalhart/TinyGPSPlus/releases/tag/v1.0.3
//This functions is from TinyGPS++
//double TinyGPSPlus::distanceBetween(double lat1, double long1, double lat2, double long2)
//This solved my problem of always overflowing. 
//Changed a bit of it to make it compatible with arduino
float Distance_Calc(float lat1, float long1, float lat2, float long2) {
 // Convert degrees to radians
  double delta = ((long1-long2)*3.14)/180;
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = (lat1*3.14)/180;
  lat2 = (lat2*3.14)/180;
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = pow(delta,2);
  delta += pow(clat2 * sdlong,2);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
  Serial.print(delta * 6372795);
}


/*
float Distance_Calc(float lat1, float lon1, float lat2, float lon2) {
 // Convert degrees to radians
  lat1 = (lat1 * 3.14159274) / 180.0;
  lon1 = (lon1 * 3.14159274) / 180.0;
  lat2 = (lat2 * 3.14159274) / 180.0;
  lon2 = (lon2 * 3.14159274) / 180.0;

  // radius of earth in metres
  float r = 6378100;
  // P
  float rho1 = r * cos(lat1);
  float z1 = r * sin(lat1);
  float x1 = rho1 * cos(lon1);
  float y1 = rho1 * sin(lon1);
  //Q
  float rho2 = r * cos(lat2);
  float z2 = r * sin(lat2);
  float x2 = rho2 * cos(lon2);
  float y2 = rho2 * sin(lon2);

  
  // Dot product
  //float dot = (x1 * x2 + y1 * y2 + z1 * z2);
  //float cos_theta = dot / (r * r);
  
  float dot_and_cos_theta = (((x1/r) *(x2/r)) + ((y1/r)*(y2/r)) + ((z1/r)*(z2/r)));
  float theta = acos(dot_and_cos_theta);
  // Distance in Metres
  
  Serial.println("value:");
  Serial.println(theta);
  return z1;
}

*/
