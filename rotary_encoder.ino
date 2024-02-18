// Needed to make button easier to work with
#include <ezButton.h>



// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4


// Used for rotary encoder
int currentStateCLK;
int lastStateCLK;


// Used for what time to set alarm to
int ghour = 0;
int gmin = 0;
bool hour_enable = true;
bool min_enable = false;

// Used for when to set alarm
bool setAlarm = false;

// Used to see how long button pressed for
unsigned long currenrButtonPress;
unsigned long lastButtonPress;

ezButton button(SW);  // create ezButton object that attach to SW pin;


void setup() {

  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // Setup Serial Monitor
  Serial.begin(9600);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
}

void loop() {
  buttonHeld();
  changeAlarm();
}

void buttonHeld() {
  // Get current state of button
  int btnState = digitalRead(SW);
  //If we detect LOW signal, button is being held
  if (btnState == LOW) {
    // See how long program has been running for
    currenrButtonPress = millis();
    // If button held for longer than two seconds and alarm is not ready to be set
    if ((currenrButtonPress - lastButtonPress > 2000) && !setAlarm) {
      button.loop();  // MUST call the loop() function first
      if (button.isReleased()) {
        // Once button is released tell user alarm ready to be set
        Serial.println("Ready to change alarm");
        // Set true so alarm can be changed
        setAlarm = true;
      }
    }
    // If button not being pressed
  } else {
    // See how long has been since last pressed
    lastButtonPress = millis();
  }
}

void changeAlarm() {
  // Check if alarm is reday to be set
  while (setAlarm) {
    // Read the current state of CLK
    currentStateCLK = digitalRead(CLK);

    // Check if button is pressed
    buttonPres();

    // Check if encoder is moved
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(DT) != currentStateCLK) {
        // If the hour enable is true then we change hours
        if (hour_enable) {
          // Decrement hour
          ghour--;
          // Loop back over to 23 if we go below 0
          if (ghour < 0) {
            ghour = 23;
          }
          // If the min enable is true then we change minutes
        } else if (min_enable) {
          // Decrement min
          gmin--;
          // Loop back over to 59 if we go below 0
          if (gmin < 0) {
            gmin = 59;
          }
        }

        // If the DT state the same as CLK state then
        // the encoder is rotating CW so increment
      } else {
        // If the hour enable is true then we change hours
        if (hour_enable) {
          // Increment hour
          ghour++;
          // Loop back over to 0 if we go above 23
          if (ghour > 23) {
            ghour = 0;
          }
          // If the min enable is true then we change minutes
        } else if (min_enable) {
          // Increment min
          gmin++;
          // Loop back over to 0 if we go below 59
          if (gmin > 59) {
            gmin = 0;
          }
        }
      }
      // Print what the current alarm will be set to
      Serial.print("Hour: ");
      Serial.print(ghour);
      Serial.print(" | Minutes: ");
      Serial.println(gmin);
    }
    // Remember last CLK state
    lastStateCLK = currentStateCLK;
  }
  // Put in a slight delay to help debounce the reading
}


void buttonPres() {
  button.loop();  // MUST call the loop() function first
  if (button.isPressed()) {
    // If user is ready to change the minutes then toggle both enables
    if (hour_enable && !min_enable) {
      // Now user can change the minutes of the alarm
      hour_enable = !hour_enable;
      min_enable = !min_enable;
      // If user presses button again then both minute and hour are set so
      // we are able to set the alarm
    } else {
      // Alarm has been set so set bool to false
      setAlarm = false;
      Serial.print("Alarm set for: ");
      // If number less than 10 print leading zero so alarm is easier to read
      if (ghour < 10) Serial.print(0);
      Serial.print(ghour);
      // If number less than 10 print leading zero so alarm is easier to read
      if (gmin < 10) Serial.print(0);
      Serial.println(gmin);
      // Reset timer variables
      currenrButtonPress = millis();
      lastButtonPress = millis();
      // Toggle both variables so next time user wants to set alarm they chnag the hour first
      hour_enable = !hour_enable;
      min_enable = !min_enable;
    }
  }
}
