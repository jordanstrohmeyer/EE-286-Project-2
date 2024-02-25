#include "SR04.h"
#define TRIG_PIN 12
#define ECHO_PIN 11
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
// long a;
long first_location, current;
long barrier = 30;
bool enable = true;

void setup() {
   Serial.begin(9600);
   delay(1000);
   first_location = sr04.Distance();
   Serial.print(first_location);
   Serial.println("first location cm");
   Serial.print(barrier);
   Serial.println("barrier cm");
   bool motion_enable= false;
  motion_sensor(enable, motion_enable);
}

void loop() {
  

  
  // delay(1000);  
          // wait a second so as not to send massive amounts of data
   // motion_sensor(enable);
   //a = sr04.Distance();
   // Serial.print(a);
   // Serial.println("cm");
   
}

bool motion_sensor (bool enable, bool motion_enable){
  // initialize function variables
  bool motion_sensor_return = false;
  long current_distance;
  unsigned long myTime;
  myTime = millis();
  while (enable == true && millis() < (myTime+30000))
  {
    current = sr04.Distance();
    Serial.print(current);
    Serial.println("current location cm");
    delay(2000);
    Serial.print(myTime);
    Serial.println("seconds");
    if (current < barrier )
    {
      motion_sensor_return = true;
      Serial.println("true");
      return motion_sensor_return;
      motion_enable= false;
      enable = false;
      
    }

  }
  
  return motion_sensor_return;// 
}

