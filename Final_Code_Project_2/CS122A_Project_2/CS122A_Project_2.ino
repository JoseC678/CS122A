#include <string.h>
#include <stdio.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


#define joyX A0
#define joyY A1







typedef struct _task{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int(*TickFct)(int);
}task;

const unsigned short numTasks = 5;
task tasks[numTasks];
int yValue; //Gloabla variable for  xposition
int xValue; //Global variable for yposition
int Alarm_On_Off = 0; //0=alarm off 1=alarm is on
int PWM_On = 0; //If the alarm is triggered this will stay on
                //Until it is turned off in the main menu or on the APP.
int GB;
int RD;

void clearMenu();
void printMenu();
void pick_Option(int, int, String);
void Alarm_is_on();
void Alarm_is_off();


void nodeMCU_Data();
String RxInput = "";
String ESP_Reset = "";
boolean InputReady = false;
String temp;

int TimerAlarm = 0;



//TickFct_JoyStick_SM1
int TickFct_JoyStick_SM1();
enum SM1_States{SM1_Start, SM1_Wait} SM1_State;

int TickFct_JoyStick_SM1(int SM1_State){
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
      // put your main code here, to run repeatedly:
      xValue = analogRead(joyX);
      yValue = analogRead(joyY);
     
      //print the values with to plot or view
      //Serial.print(xValue);
      //Serial.print("\t");
      //Serial.println(yValue);
      break;
    default:
      break;
  }

  return SM1_State;
}


//TickFct_Menu_SM2
int TickFct_Menu_SM2();
enum SM2_States{SM2_Start,AL_OnOff  , Timer_OnOff  , SetTimer  , ResetAlarm //First Menu
                         ,AL_OnOff_R, Timer_OnOff_R, SetTimer_R, ResetAlarm_RR} SM2_State;//Second menu option
//https://lastminuteengineers.com/nokia-5110-lcd-arduino-tutorial/
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
int rotatetext = 1;

const byte MagnetSens = 8; //Door sensor
const byte GreenButton = 10; //Turn system on
const byte RedButton = 11; //Turn system off
const byte LED = 9; //Test LED 
const byte Flash_RED_LED = 12; //Test LED 
String startingMess = "OFF";
String FrontLight = "OFF";
String BackLight = "OFF";

int TickFct_Menu_SM2(int SM2_State){
  switch (SM2_State){
    case SM2_Start:
      SM2_State = AL_OnOff;
      break;
    /**********************FIRST*OPTION*MENU********************************************/
    case AL_OnOff://FIRST OPTION
      if(xValue>800 && (yValue>300 && yValue<600)){//SCROLL DOWN
        printMenu();
        SM2_State = Timer_OnOff;
      }else if(xValue<200 && (yValue>300 && yValue<600)){//SCROLL UP
        printMenu();
        SM2_State = ResetAlarm;
      }else if ((xValue>300 && xValue<600) && yValue>800){ //SCROLL RIGHT
        clearMenu();
        SM2_State = AL_OnOff_R;
      }else{
        SM2_State = AL_OnOff;
      }
      break;
      
    case Timer_OnOff: //SECOND OPTION 
      if(xValue>800 && (yValue>300 && yValue<600)){//SCROLL DOWN
        printMenu();
        SM2_State = SetTimer;
      }else if(xValue<200 && (yValue>300 && yValue<600)){//SCROLL UP
        printMenu();
        SM2_State = AL_OnOff;
      }else if ((xValue>300 && xValue<600) && yValue>800){ //SCROLL RIGHT
        clearMenu();
        SM2_State = Timer_OnOff_R;
      }else{
        SM2_State = Timer_OnOff;
      }
      break;
      
    case SetTimer: //THIRD OPTION
      if(xValue>800 && (yValue>300 && yValue<600)){//SCROLL DOWN
        printMenu();
        SM2_State = ResetAlarm;
      }else if(xValue<200 && (yValue>300 && yValue<600)){//SCROLL UP
        printMenu();
        SM2_State = Timer_OnOff;
      }else if ((xValue>300 && xValue<600) && yValue>800){ //SCROLL RIGHT
        clearMenu();
        SM2_State = SetTimer_R;
      }else{
        SM2_State = SetTimer;
      }
      break;
      
    case ResetAlarm: //FOURTH OPTION
      if(xValue>800 && (yValue>300 && yValue<600)){//SCROLL DOWN
        printMenu();
        SM2_State = AL_OnOff;
      }else if(xValue<200 && (yValue>300 && yValue<600)){//SCROLL UP
        printMenu();
        SM2_State = SetTimer;
      }else if ((xValue>300 && xValue<600) && yValue>800){ //SCROLL RIGHT
        clearMenu();
        SM2_State = ResetAlarm_RR;
        //Print_Line(0, 8,"Green=Reset Alarm");
      }else{
        SM2_State = ResetAlarm;
      }
      break;
    /**********************SECOND*OPTION*MENU*FOR*THE*OPTION*SELECTED*******************/
    case AL_OnOff_R:
      //Controls for the menu
      if((xValue>300 && xValue<600) && yValue<200){//SCROLL LEFT
        SM2_State = AL_OnOff;
        printMenu();
      }else{
        SM2_State = AL_OnOff_R;
      }
      break;
    case ResetAlarm_RR:
      //Controls for the menu
      if((xValue>300 && xValue<600) && yValue<200){//SCROLL LEFT
        SM2_State = ResetAlarm;
        printMenu();
      }else{
        SM2_State = ResetAlarm_RR;
      }
      //Serial.println("sdasasasds");
      break;
    case Timer_OnOff_R:
      //Controls for the menu
      if((xValue>300 && xValue<600) && yValue<200){//SCROLL LEFT
        SM2_State = Timer_OnOff;
        printMenu();
      }else{
        SM2_State = Timer_OnOff_R;
      }
      //Serial.println("sdasasasds");
    break;
    case SetTimer_R:
      //Controls for the menu
      if((xValue>300 && xValue<600) && yValue<200){//SCROLL LEFT
        SM2_State = SetTimer;
        printMenu();
      }else{
        SM2_State = SetTimer_R;
      }
      //Serial.println("sdasasasds");
    break;
    default:
      SM2_State = AL_OnOff;
      break;
  }



  
  switch (SM2_State){
    case SM2_Start:
      break;
      
    case AL_OnOff:
      pick_Option(0, 0, "Alarm On/Off");
      break;
      
    case Timer_OnOff:
      pick_Option(0, 8, "Front Light");
      break;
      
    case SetTimer:
      pick_Option(0, 16, "Back Light");
      break;
    case ResetAlarm:
      pick_Option(0, 24, "Reset Alarm");
      break;
    /**********************SECOND*OPTION*MENU*FOR*THE*OPTION*SELECTED*******************/
    case AL_OnOff_R:
      GB = digitalRead(GreenButton);
      RD = digitalRead(RedButton);
      if (GB == LOW) {
        Alarm_On_Off = 1;
        startingMess = "ON ";
      }else if(RD == LOW){
        startingMess = "OFF";
        Alarm_On_Off = 0;
      }

      pick_Option(0, 0,startingMess);
      Print_Line(0, 8,"Green=AlarmOn");
      Print_Line(0, 16,"Red=Alarm Off");
      
      break;
      
    case ResetAlarm_RR:
      Print_Line(0, 4,"Green = Reset Alarm");
      
      GB = digitalRead(GreenButton);
      if (GB == LOW) {
        PWM_On = 0;

      }
      break;
    case Timer_OnOff_R:
      GB = digitalRead(GreenButton);
      RD = digitalRead(RedButton);
      if (GB == LOW) {
        digitalWrite(2, HIGH);/////////////////////////
        FrontLight = "ON ";
      }else if(RD == LOW){
        FrontLight = "OFF";
        digitalWrite(2, LOW);
      }

      pick_Option(0, 0,FrontLight);
      Print_Line(0, 8,"Green=LightOn");
      Print_Line(0, 16,"Red=LightOff");
    
    break;
    case SetTimer_R:
      GB = digitalRead(GreenButton);
      RD = digitalRead(RedButton);
      if (GB == LOW) {
        digitalWrite(13, HIGH);/////////////////////////
        BackLight = "ON ";
      }else if(RD == LOW){
        BackLight = "OFF";
        digitalWrite(13, LOW);
      }

      pick_Option(0, 0,BackLight);
      Print_Line(0, 8,"Green=LightOn");
      Print_Line(0, 16,"Red=LightOff");
    
    break;
    default:
      break;
  }

  return SM2_State;
}



//TickFct_Alarm_SM3
int TickFct_Alarm_SM3();
enum SM3_States{SM3_Start, SM3_Wait} SM3_State;

int TickFct_Alarm_SM3(int SM3_State){
  switch (SM3_State){
    case SM3_Start:
      SM3_State = SM3_Wait;
      break;
    case SM3_Wait:
      SM3_State = SM3_Wait;
      break;
    default:
      SM3_State = SM3_Wait;
      break;
  }
  switch (SM3_State){
    case SM3_Start:
      break;
    case SM3_Wait:
      if(Alarm_On_Off == 0){
        Alarm_is_off();
      }else if(Alarm_On_Off == 1){
        Alarm_is_on();
      }else{
        SM3_State = SM3_Wait;
      }
      break;

    break;
    default:
      break;
  }

  return SM3_State;
}


//TickFct_PWM_SM4
int TickFct_PWM_SM4();
enum SM4_States{SM4_Start, SM4_Wait, SM4_BlinkOn, SM4_BlinkOff} SM4_State;

int TickFct_PWM_SM4(int SM4_State){
  switch (SM4_State){
    case SM4_Start:
      SM4_State = SM4_Wait;
      break;
    case SM4_Wait:
      if(PWM_On == 1){
        SM4_State = SM4_BlinkOn;
      }else{
        SM4_State = SM4_Wait;
      }
      break;
    case SM4_BlinkOn:
      if(PWM_On == 0){
        SM4_State = SM4_Wait;
      }else{
        SM4_State = SM4_BlinkOff;
      }
      break;
    case SM4_BlinkOff:
      if(PWM_On == 0){
        SM4_State = SM4_Wait;
      }else{
        SM4_State = SM4_BlinkOn;
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
      digitalWrite(Flash_RED_LED, LOW);
      break;
    case SM4_BlinkOn:
      digitalWrite(Flash_RED_LED, HIGH);
      break;
    case SM4_BlinkOff:
      digitalWrite(Flash_RED_LED, LOW);
      break;
      
    break;
    default:
      break;
  }

  return SM4_State;
}



//TickFct_NODEMCU_SM5
int TickFct_NODEMCU_SM5();
enum SM5_States{SM5_Start, SM5_Wait} SM5_State;

int TickFct_NODEMCU_SM5(int SM5_State){
  switch (SM5_State){
    case SM5_Start:
      SM5_State = SM5_Wait;
      break;
    case SM5_Wait:
        SM5_State = SM5_Wait;
      break;

    default:
      SM5_State = SM5_Wait;
      break;
  }
  switch (SM5_State){
    case SM5_Start:
      break;
    case SM5_Wait:
      //Serial.println("ITWORKS");
      nodeMCU_Data();
      break;
      
    break;
    default:
      break;
  }

  return SM5_State;
}



void setup() {
    //Set up the Values and period
    unsigned char i=0;
    tasks[i].state= SM1_Start;
    tasks[i].period=100;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_JoyStick_SM1;

    i++;
    tasks[i].state= SM2_Start;
    tasks[i].period=150;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_Menu_SM2;

    i++;
    tasks[i].state= SM3_Start;
    tasks[i].period=100;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_Alarm_SM3;


    i++;
    tasks[i].state= SM4_Start;
    tasks[i].period=500;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_PWM_SM4;

    i++;
    tasks[i].state= SM5_Start;
    tasks[i].period=50;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TickFct_NODEMCU_SM5;
    
    Serial.begin(115200);

    

    //Nokia Screen Initialization
    display.begin();
    display.setContrast(20);
    display.clearDisplay();
    printMenu();

  //Buttons and ledoutputs to show if alarm is on or off
  pinMode(Flash_RED_LED,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(MagnetSens, INPUT_PULLUP);
  pinMode(GreenButton, INPUT_PULLUP);
  pinMode(RedButton, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

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


////ADD PWM MACHINE FOR BLINKING LIGHTS

void nodeMCU_Data(){
    if (InputReady) {
        temp = RxInput.substring(0, 2); //get the ID
        //Serial.println(temp);
        if (temp == "al") { //Check if Reset
          ESP_Reset = RxInput.substring(6, 8); //The longitude starts at 17

          if(ESP_Reset == "on"){
            startingMess = "ON ";
            Alarm_On_Off=1;
            //Serial.print("Reset=on");
          }else if(ESP_Reset == "of"){
            startingMess = "OFF ";
            Alarm_On_Off=0;
          }

        }

        if (temp == "re") { //Check if its alarm on off
          ESP_Reset = RxInput.substring(6, 8); //The longitude starts at 17
          if(ESP_Reset == "on"){
            PWM_On = 0;
          }


        }

        if (temp == "fr") { //Check if Front light is on
          ESP_Reset = RxInput.substring(6, 8); //The longitude starts at 17

          if(ESP_Reset == "on"){
            FrontLight = "ON ";
            Serial.print("Front=on");
            digitalWrite(2, HIGH);
          }else if(ESP_Reset == "of"){
            FrontLight = "OFF";
            Serial.print("Front=off");
            digitalWrite(2, LOW);
          }

        }

        if (temp == "ba") { //Check if Back light is on
          ESP_Reset = RxInput.substring(6, 8); //The longitude starts at 17

          if(ESP_Reset == "on"){
            BackLight = "ON";
            Serial.print("Back=on");
            digitalWrite(13, HIGH);
          }else if(ESP_Reset == "of"){
            BackLight = "OFF";
            Serial.print("Back=off");
            digitalWrite(13, LOW);
          }

        }
        //Serial.println(temp);
        if (temp == "cl") { //Check if Back light is on
          ESP_Reset = RxInput.substring(6, 8); //The longitude starts at 17
          //Serial.println("ALARM");
          if(ESP_Reset == "on"){
            Serial.println("ALARM=ON");
            Alarm_On_Off = 1;
          }else if(ESP_Reset == "of"){
            Serial.println("ALARM=OFF");
            Alarm_On_Off = 0;
          }

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

}


void Alarm_is_on(){
  digitalWrite(LED, HIGH);
  
  int sensorVal = digitalRead(MagnetSens);
  //Serial.println(sensorVal);
  
  if (sensorVal == HIGH) {
    //digitalWrite(Flash_RED_LED, HIGH); //Add the global variable for PWM State 
                                       //So when the alarm goes on it stays on
                                       //Till turned off in the menu
    PWM_On = 1;
  }
  

}

void Alarm_is_off(){
  digitalWrite(Flash_RED_LED, LOW);
  digitalWrite(LED, LOW);
  int sensorVal = digitalRead(MagnetSens);
  //Serial.println(sensorVal);
}


void clearMenu(){
    display.clearDisplay();
    display.display();
}

void pick_Option(int column,int row, String text ){
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.setCursor(column,row);
  display.println(text);
  display.display();
}

void Print_Line(int column,int row, String text ){
  display.setTextSize(1);
  display.setTextColor(BLACK); // 'inverted' text
  display.setCursor(column,row);
  display.println(text);
  display.display();
}




void printMenu(){
  display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(0,0);
      display.println("Alarm On/Off");
      display.setCursor(0,8);
      display.println("Front Light");
      display.setCursor(0,16);
      display.println("Back Light");
      display.setCursor(0,24);
      display.println("Reset Alarm");
      display.display();
      

}
