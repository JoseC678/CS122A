#include <Wire.h>

typedef struct _task{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int(*TickFct)(int);
}task;

//TickFct_HBEAT
enum SM1_States{SM1_Start, SM1_Wait, SM1_Emergency} SM1_State;
unsigned char BPM = 0;


static task task1;
task *tasks[] = {&task1}; 
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);




void setup() {
    //Set up the Values and period
    task1.state= SM1_Start;
    task1.period=500;
    task1.elapsedTime = 0;
    task1.TickFct = &TickFct_HBEAT;
    
    Serial.begin(9600);
    Serial.println("heart rate sensor:");
    Wire.begin();
}

int TickFct_HBEAT(){
  switch (SM1_State){
    case SM1_Start:
      SM1_State = SM1_Wait;
      break;
    case SM1_Wait:

      SM1_State = SM1_Wait;
      
      break;
    case SM1_Emergency:
      SM1_State = SM1_Emergency;
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
          Serial.println(BPM, DEC);         // print heart rate value
      }
      break;
    case SM1_Emergency:
      break;
    default:
      break;
  }
}



void loop() {
  unsigned short i;
        for (i = 0; i < numTasks; ++i) {
            if (tasks[i]->elapsedTime >= tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
        tasks[i]->elapsedTime += 1000;
        }
    delay(1000);
   }
