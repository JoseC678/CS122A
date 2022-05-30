#include <SoftwareSerial.h>

String str;
void setup(){
Serial.begin(115200);
Serial1.begin(115200);
delay(2000);
}
void loop()
{
str = "coming from ESP8266: ";
Serial1.println(str);
delay(1000);
}
