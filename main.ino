//www.elegoo.com
//2016.12.08
#include "SR04.h"
#include <ezButton.h>
#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>


#define TRIG_PIN 12
#define ECHO_PIN 11

// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4


// Used for rotary encoder
int currentStateCLK;
int lastStateCLK;
int gtime_hour;
int gtime_minute;

// Used for what time to set alarm to
int ahour = 0;
int amin = 0;
bool hour_enable = true;
bool min_enable = false;
bool motion_enable = false;

// Used for when to set alarm
bool setAlarm = false;
// Used to enable the motion sensor
bool motionSensoEnable = false;
bool ButtonPressEn = false;

// Used to see how long button pressed for
unsigned long currenrButtonPress;
unsigned long lastButtonPress;

// Used to see how long has been since last time check
unsigned long lastClockCylce;


SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
ezButton button(SW);  // create ezButton object that attach to SW pin;
DS3231 clock;
RTCDateTime dt;
LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup() {
  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
  clock.begin();
  //initialize lcd screen
  lcd.init();
  // turn on the backlight
  lcd.backlight();
  lcd.clear();
}

void loop() {
  button.loop();  // MUST call the loop() function first

  clockFun();
  buttonHeld();
  changeAlarm();
  checkAlarm();
}
void buttonHeld() {
  //If we detect LOW signal, button is being held
  if (button.getState() == 0) {
    // See how long program has been running for
    currenrButtonPress = millis();
    // If button held for longer than two seconds and alarm is not ready to be set
    if ((currenrButtonPress - lastButtonPress > 1000) && setAlarm == false) {
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(1, 2);
      // Once button is released tell user alarm ready to be set
      lcd.print("Ready to set alarm");
      // Set true so alarm can be changed
      setAlarm = true;
    }
    // If button not being pressed
  } else {
    // See how long has been since last pressed
    lastButtonPress = millis();
  }
}


void buttonPres() {
  if (ButtonPressEn == true) {
    if (button.isReleased()) {
      Serial.println("Button pressed!");

      // If user is ready to change the minutes then toggle both enables
      if (hour_enable == true && min_enable == false) {
        Serial.println("  edwbydwbdwb");
        // Now user can change the minutes of the alarm
        hour_enable = false;
        min_enable = true;
        // If user presses button again then both minute and hour are set so
        // we are able to set the alarm
      } else {
        // Alarm has been set so set bool to false
        setAlarm = false;
        // Reset timer variables
        currenrButtonPress = millis();
        lastButtonPress = millis();
        // Toggle both variables so next time user wants to set alarm they chnag the hour first
        hour_enable = true;
        min_enable = false;
        ButtonPressEn = false;
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
      }
    }
  }
}

void changeAlarm() {

  if (setAlarm) {
    currentStateCLK = digitalRead(CLK);

    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(DT) != currentStateCLK) {
        Serial.println("CCW");

        if (hour_enable) {
          // Decrement hour
          ahour--;
          // Loop back over to 23 if we go below 0
          if (ahour < 0) ahour = 23;
          // If the min enable is true then we change minutes
        } else if (min_enable) {
          // Decrement min
          amin--;
          // Loop back over to 59 if we go below 0
          if (amin < 0) amin = 59;
        }
      } else {
        // Encoder is rotating CW so increment
        Serial.println("CW");
        // If the hour enable is true then we change hours
        if (hour_enable) {
          // Increment hour
          ahour++;
          // Loop back over to 0 if we go above 23
          if (ahour > 23) ahour = 0;
          // If the min enable is true then we change minutes
        } else if (min_enable) {
          // Increment min
          amin++;
          // Loop back over to 0 if we go below 59
          if (amin > 59) amin = 0;
        }
      }
      Serial.println(hour_enable);
      Serial.println(min_enable);
      ButtonPressEn = true;

      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(0, 3);
      lcd.print("                    ");
      lcd.setCursor(0, 2);
      lcd.print("Hour: ");
      lcd.print(ahour);
      lcd.setCursor(0, 3);
      lcd.print("Minutes: ");
      lcd.print(amin);
    }
    buttonPres();
    // Remember last CLK state
    lastStateCLK = currentStateCLK;
  }
}
void clockFun() {
  if (setAlarm == false) {
    dt = clock.getDateTime();
    unsigned long clockTimer = millis();
    if ((clockTimer - lastClockCylce) > 1000) {
      lastClockCylce = clockTimer;
      lcd.setCursor(5, 0);
      lcd.print(dt.year);
      lcd.print("-");
      if (dt.month < 10) lcd.print(0);
      lcd.print(dt.month);
      lcd.print("-");
      lcd.print(dt.day);
      lcd.setCursor(5, 1);
      lcd.print(" ");
      if (dt.hour < 10) lcd.print(0);
      lcd.print(dt.hour);
      lcd.print(":");
      if (dt.minute < 10) lcd.print(0);
      lcd.print(dt.minute);
      lcd.print(":");
      if (dt.second < 10) lcd.print(0);
      lcd.print(dt.second);
      lcd.setCursor(4, 2);
      lcd.print("Alarm: ");
      if (ahour < 10) lcd.print(0);
      lcd.print(ahour);
      if (amin < 10) lcd.print(0);
      lcd.print(amin);

      gtime_minute = dt.minute;
      gtime_hour = dt.hour;
    }
  }
}

void checkAlarm() {
  if (setAlarm == false) {
    if (ahour == gtime_hour && amin == gtime_minute) {
      alarm_enable = true;
    }
  }
}
