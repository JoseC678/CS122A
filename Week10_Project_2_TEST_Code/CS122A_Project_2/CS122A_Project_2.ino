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

const unsigned short numTasks = 3;
task tasks[numTasks];
int yValue; //Gloabla variable for  xposition
int xValue; //Global variable for yposition
int Alarm_On_Off = 0; //0=alarm off 1=alarm is on

void clearMenu();
void printMenu();
void pick_Option(int, int, String);
void Alarm_is_on();
void Alarm_is_off();


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
enum SM2_States{SM2_Start,AL_OnOff  , Timer_OnOff  , SetTimer  , DoorsOpen //First Menu
                         ,AL_OnOff_R, Timer_OnOff_R, SetTimer_R, DoorsOpen_R} SM2_State;//Second menu option
//https://lastminuteengineers.com/nokia-5110-lcd-arduino-tutorial/
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
int rotatetext = 1;

const byte MagnetSens = 8; //Door sensor
const byte GreenButton = 10; //Turn system on
const byte RedButton = 11; //Turn system off
const byte LED = 9; //Test LED 
const byte Flash_RED_LED = 12; //Test LED 

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
        SM2_State = DoorsOpen;
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
      }else{
        SM2_State = Timer_OnOff;
      }
      break;
      
    case SetTimer: //THIRD OPTION
      if(xValue>800 && (yValue>300 && yValue<600)){//SCROLL DOWN
        printMenu();
        SM2_State = DoorsOpen;
      }else if(xValue<200 && (yValue>300 && yValue<600)){//SCROLL UP
        printMenu();
        SM2_State = Timer_OnOff;
      }else{
        SM2_State = SetTimer;
      }
      break;
      
    case DoorsOpen: //FOURTH OPTION
      if(xValue>800 && (yValue>300 && yValue<600)){//SCROLL DOWN
        printMenu();
        SM2_State = AL_OnOff;
      }else if(xValue<200 && (yValue>300 && yValue<600)){//SCROLL UP
        printMenu();
        SM2_State = SetTimer;
      }else{
        SM2_State = DoorsOpen;
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

      //Controls for the alarm on/off system
      
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
      pick_Option(0, 8, "Timer On/Off");
      break;
      
    case SetTimer:
      pick_Option(0, 16, "Set Timer");
      break;
    case DoorsOpen:
      pick_Option(0, 24, "Doors Open");
      break;
    /**********************SECOND*OPTION*MENU*FOR*THE*OPTION*SELECTED*******************/
    case AL_OnOff_R:
      int GB = digitalRead(GreenButton);
      int RD = digitalRead(RedButton);
      if (GB == LOW) {
        Alarm_On_Off = 1;
      }else if(RD == LOW){
        Alarm_On_Off = 0;
      }
      
      pick_Option(0, 0, "ON");
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
    Serial.begin(115200);

    

    //Nokia Screen Initialization
    display.begin();
    display.setContrast(20);
    display.clearDisplay();
    printMenu();

  //Buttons and ledoutputs to show if alarm is on or off
  pinMode(Flash_RED_LED,OUTPUT);
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

void Alarm_is_on(){
  digitalWrite(LED, HIGH);
  
  int sensorVal = digitalRead(MagnetSens);
  Serial.println(sensorVal);
  
  if (sensorVal == HIGH) {
    digitalWrite(Flash_RED_LED, HIGH);
  } else {
    digitalWrite(Flash_RED_LED, LOW);
  }
  

}

void Alarm_is_off(){
  digitalWrite(Flash_RED_LED, LOW);
  digitalWrite(LED, LOW);
  int sensorVal = digitalRead(MagnetSens);
  Serial.println(sensorVal);
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



void printMenu(){
  display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.setCursor(0,0);
      display.println("Alarm On/Off");
      display.setCursor(0,8);
      display.println("Timer On/Off");
      display.setCursor(0,16);
      display.println("Set Timer");
      display.setCursor(0,24);
      display.println("Doors Open");
      display.display();
      

}
