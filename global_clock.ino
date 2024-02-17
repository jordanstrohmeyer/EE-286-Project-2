//www.elegoo.com
//2018.10.24
#include <Wire.h>
#include <DS3231.h>
int gtime_hour;
int gtime_minute;
DS3231 clock;
RTCDateTime dt;

void setup()
{
  Serial.begin(9600);

  Serial.println("Initialize RTC module");
  // Initialize DS3231
  clock.begin();

  
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__);    
  /*
  Tips:This command will be executed every time when Arduino restarts. 
       Comment this line out to store the memory of DS3231 module
  */
}

void loop()
{
  dt = clock.getDateTime();
  // Set value of global variables 
  gtime_minute=dt.minute;
  gtime_hour=dt.hour;
  Serial.print("global Time: "); Serial.print(gtime_hour);Serial.print(":");Serial.println(gtime_minute);

  // For leading zero look to DS3231_dateformat example

  Serial.print("Time: ");
  
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute);
  Serial.println();
 
  // Assigning Global variabels for time in hours and minutes 
  /*gtime_hour=dt.hour;
  gtime_minute=dt.minute;
  Serial.print(gtime_hour);
  */
  delay(1000);

}
  
