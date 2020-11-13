/*
 Author: Yunyue Li
 Date: 13.11.2020
 Assignment 3: Car system
 */

#include <DHT11.h>
#include <Display.h>
#include <TM1637Display.h>

#define POT A0
#define NTC A1
#define LDR A2
#define BTN_LEFT 9
#define BTN_RIGHT 8
#define LED_YELLOW 7
#define LED_BLUE 6
#define LED_GREEN 5
#define LED_RED 4
#define BUTTON_DELAY  50 // ms
#define LED_INTERVAL 1000 // blink every 1 sec
#define TURN_DELAY 50
#define NTC_DELAY 5000 // get new temp every 5 sec
#define LDR_DELAY 2000 //delay for 2 sec, in case there is a cloud or shadow 

#define NTC_R25  10000              // the resistance of the NTC at 25'C is 10k ohm
#define NTC_MATERIAL_CONSTANT  3950 

int lastBtnLeftState = HIGH;
int lastBtnRightState = HIGH;
unsigned long lastTimeBtnPressed = 0;

int ledState = HIGH;
unsigned long lastTimeBlinked = 0;

unsigned long lastTurn = 0;
unsigned long lastNTC = 0;
unsigned long lastLDR = 0;

bool goLeft = false;
bool goRight = false;

enum Steering {
  left,
  right
};

int prevSteering = -1; //avoid conflict

enum Mode {
  normal,
  hazard
};

int carMode = Mode::normal;
int prevCarMode = Mode::normal;

void setup() {
  Serial.begin(9600);

  pinMode(POT, INPUT);
  pinMode(NTC, INPUT);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Display.clear();
}

void loop() {
  // NTC debounce setting
  if (millis() - lastNTC > NTC_DELAY) {
    lastNTC = millis();

    Serial.println("temp");
    Serial.println(getTemperature());
  }
  
  // LDR debounce setting
  if (millis() - lastLDR > LDR_DELAY) {
    lastLDR = millis();

    bool lightEnviro = map(analogRead(LDR), 0, 200, 0, 1); 
    String headlight;                                      

    if (!lightEnviro) {
      digitalWrite(LED_GREEN, HIGH);
      headlight = "ON";
    } else if (lightEnviro) {
      digitalWrite(LED_GREEN, LOW);
      headlight = "OFF";
    }
    
    Serial.println("headlight");
    Serial.println(headlight);
  }

  // button
  if (isButtonPressed(BTN_LEFT, lastBtnLeftState)) {
    if (!goLeft) {
      sendTurnMessageLeft();  //only send message once to serial port right after pressing the button, 
    }                         //instead of sending messages during debounce time. 
    
    goLeft = true;
    goRight = false;
  } else if (isButtonPressed(BTN_RIGHT, lastBtnRightState)) {
    if (!goRight) {
      sendTurnMessageRight();
    }
    
    goLeft = false;
    goRight = true;
  }

  // pot debounce setting
  if (millis() - lastTurn > TURN_DELAY) {
    lastTurn = millis();

    int steering = map(analogRead(POT), 0, 1024, 0, 2);

    if (steering == Steering::left) {
      if (prevSteering == Steering::right) {
        goRight = false;

        if (carMode == Mode::normal) {
          digitalWrite(LED_BLUE, LOW);    //streeing controls the leds. 
        }                                 //After pressing the button, the blue led blinks. It stops blinking until the streeing switch from right to left
      }
    } else if (steering == Steering::right) {
      if (prevSteering == Steering::left) {
        goLeft = false;
        
        if (carMode == Mode::normal) {
          digitalWrite(LED_YELLOW, LOW);
        }
      }
    }
    
    prevSteering = steering;
  }

  if (carMode == Mode::normal) {
    if (prevCarMode == Mode::hazard) {   //in case the leds state remains on when switch from hazard to normal
      setAlarm(false);
    }
  
    if (millis() - lastTimeBlinked > LED_INTERVAL) {
      lastTimeBlinked = millis();
      
      if (goLeft) {
        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, ledState);
      } else if (goRight) {
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_BLUE, ledState);
      }
      
      ledState = !ledState;
    }
  }

  if (carMode == Mode::hazard) {
    if (millis() - lastTimeBlinked > LED_INTERVAL) {
      lastTimeBlinked = millis();
      setAlarm(ledState);
      ledState = !ledState;
    }
  }

  prevCarMode = carMode;

  if (Serial.available()){
    String command = Serial.readStringUntil('\n');
    String alarmState;
    
    if (command == "alarm") {
      carMode = Mode::hazard;
      alarmState = "Alarm On";
    } else if (command == "noalarm") {
      carMode = Mode::normal;
      alarmState = "Alarm Off";
    }

    Serial.println("alarmState");
    Serial.println(alarmState);
  }
}

float getTemperature(){
  int value = analogRead(NTC); //Temperature app
  float resistance = (float)value * NTC_R25 / (1024 - value); // Calculate resistance
  return 1 / (log(resistance / NTC_R25) / NTC_MATERIAL_CONSTANT + 1 / 298.15) - 273.15;
}

bool isButtonPressed(int pinNumber, int &lastButtonState) {
  int buttonState = digitalRead(pinNumber);

  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    lastTimeBtnPressed = millis();
  }

  if (millis() - lastTimeBtnPressed > BUTTON_DELAY) {
    if (buttonState == LOW) {
      return true;
    }
  }
  
  return false;
}

void setAlarm(bool state) {
  digitalWrite(LED_YELLOW, state);
  digitalWrite(LED_BLUE, state);
  digitalWrite(LED_RED, state);
}

void sendTurnMessageLeft() {
  Serial.println("turn");
  Serial.println("Turn Left");
}

void sendTurnMessageRight() {
  Serial.println("turn");
  Serial.println("Turn Right");
}
