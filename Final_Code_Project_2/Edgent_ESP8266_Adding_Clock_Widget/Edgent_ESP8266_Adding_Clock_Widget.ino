
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPL9kRtsR9k"
#define BLYNK_DEVICE_NAME "NodeMCU"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD

#include "BlynkEdgent.h"
#include <TimeLib.h>

bool timerSet = false; //Used to determine if the stop timer is properly set
bool timerButton = false; //Used to determine if the stop timer button is on

bool AlarmOn = false;
bool AlarmOff = false;

int currentHour;
int currentMinute;
int currentSecond;
int currentDay;

int startHour;
int startMinute;
int startSecond;

int stopHour;
int stopMinute;
int stopSecond;

int daysSet[7];

unsigned long int elapsedTime;


void checkTimer_app();
String returnDay(int);

//BlynkTimer timer; already created in BlynkEdgent.h

BLYNK_WRITE(V4)
{
  if(param.asInt()==1){
    //digitalWrite(2, HIGH);
    Serial1.println("front,on");
  }
  else{
    //digitalWrite(2, LOW);
    Serial1.println("front,off");
  }
}

BLYNK_WRITE(V5)
{
  if(param.asInt()==1){
    //digitalWrite(2, HIGH);
    Serial1.println("backk,on");
  }
  else{
    //digitalWrite(2, LOW);
    Serial1.println("backk,off");
  }
}

BLYNK_WRITE(V0)
{
  if(param.asInt()==1){
    //digitalWrite(2, HIGH);
    Serial1.println("alarm,on");
  }
  else{
    //digitalWrite(2, LOW);
    Serial1.println("alarm,off");
  }
}

BLYNK_WRITE(V1)
{
  if(param.asInt()==1){
    //digitalWrite(2, HIGH);
    Serial1.println("reset,on");
  }
  else{
    //digitalWrite(2, LOW);
    Serial1.println("reset,off");
  }
}


BLYNK_WRITE(V2){

  if(param.asInt()==1){
    timerButton = true;
  }
  else{
    timerButton = false;
  }
}



BLYNK_WRITE(V3){//Get Time input of start time and stop time 
  unsigned char week_day;
  TimeInputParam t(param);

  startHour = t.getStartHour();
  startMinute = t.getStartMinute();
  startSecond = t.getStartSecond();

  
  stopHour = t.getStopHour();
  stopMinute = t.getStopMinute();
  stopSecond = t.getStopSecond();


  if (t.hasStartTime() && t.hasStopTime() ){ //The timer is properly set
    //Reset the days to assign the correct days.
    daysSet[0]=0; daysSet[1]=0; daysSet[2]=0; daysSet[3]=0; daysSet[4]=0; 
    daysSet[5]=0; daysSet[6]=0; daysSet[7]=0; 
    
    Serial.println(String("Start Time: ")+startHour+":"+startMinute+":"+startSecond);               
    Serial.println(String("Stop Time: ")+stopHour+":"+stopMinute+":"+stopSecond);
                   
    for (int i = 1; i <= 7; i++){
      if (t.isWeekdaySelected(i)){//Check to see which days are selected
        daysSet[i] = 1; //Set true for that day that corresponds 1=Monday 7=Sunday
        week_day |= (0x01 << (i-1));
        //Serial.println(String("Day ") + i + " is selected");
        
      }else{
        week_day &= (~(0x01 << (i-1)));
      }
    }

    Serial.println(String("Days: ")+daysSet[0]+","+daysSet[1]+","+daysSet[2]+","+daysSet[3]+","
                                   +daysSet[4]+","+daysSet[5]+","+daysSet[6]+","+daysSet[7]);
    timerSet = true;
    Serial.print("set: ");
    Serial.println(timerSet);

  }else{//The timer is not properly set
    timerSet = false;
    Serial.print("Not set: ");
    Serial.println(timerSet);
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

BLYNK_WRITE(InternalPinRTC) //InternalPinRTC which is the system pin to store time
{
  currentHour = hour();
  currentMinute = minute();
  currentSecond = second ();
  currentDay = weekday();


  if( currentDay == 1 ){ //the library has sunday as 1 but on the app its monday have to 
    currentDay = 7;      //Shift it back by 1
  }else{
      currentDay -= 1; 
  }
  unsigned long t = param.asLong(); 
  setTime(t); //Unix value time that has been recieved 
  Serial.println("");
  Serial.println(String("Current Time: ") + currentHour + ":" + currentMinute + ":" + currentSecond);
  Serial.println(String("Current Day: ") + returnDay(currentDay)); 

}


void checkTime() 
{
  Blynk.sendInternal("rtc", "sync"); //o sync your device's time with Blynk Command
}


void setup()
{
  pinMode(2, OUTPUT); // Initialise digital pin 2 as an output pin
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(100);

  BlynkEdgent.begin();
  timer.setInterval(1000, checkTime); //Check every 1 seconds
  elapsedTime=0;
}


void loop() 
{
  BlynkEdgent.run();
  timer.run();

  if ((millis() - elapsedTime) >= 1000) { //Tick every second
    if(timerSet==true && timerButton==true){
      Serial.println("TIMER ON");
      checkTimer_app();
      elapsedTime = millis();      
    }
  }

}

/*
int currentHour;
int currentMinute;
int currentSecond;
int currentDay;

int startHour;
int startMinute;
int startSecond;

int stopHour;
int stopMinute;
int stopSecond;

*/

void checkTimer_app(){
  
  if((currentHour==startHour) && (currentMinute==startMinute) && (daysSet[currentDay])){
    Serial.println("TimerOn");
    Serial1.println("clock,on");
    
  }

  if((currentHour==stopHour) && (currentMinute==stopMinute) && (daysSet[currentDay])){
    Serial.println("TimerOff");
    Serial1.println("clock,off");
  }
}

String returnDay(int currentDay){
  if(currentDay==1){return "Monday";}
  else if(currentDay==2){return "Tuesday";}
  else if(currentDay==3){return "Wednesday";}
  else if(currentDay==4){return "Thursday";}
  else if(currentDay==5){return "Friday";}
  else if(currentDay==6){return "Saturday";}
  else {return "Sunday";}
}
