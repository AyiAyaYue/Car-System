/*
 Author: Yunyue Li
 Date: 11.11.2020
 Assignment 3: Car system
 */
#include "Display.h"
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
#define LED_INTERVAL 1000 // ms
#define TURN_DELAY 50
#define NTC_DELAY 5000
#define LDR_DELAY 50

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

int oldSteering = -1;

enum mode {
  normal,
  hazard
};

int carMode = 0;

String turn;

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
}




void loop() {
  if (millis() - lastNTC > NTC_DELAY) {
    lastNTC = millis();

    float temp = get_temperature();
    Serial.println("temp");
    Serial.println(temp);
  }

  if (millis() - lastLDR > LDR_DELAY){
    lastLDR = millis();

    bool darkEnviro = map(analogRead(LDR), 0, 500, 0, 1);
    String headlight;

    if (darkEnviro) {
      digitalWrite(LED_GREEN, HIGH);
      headlight = "ON";
    } else if (!darkEnviro) {
      digitalWrite(LED_GREEN, LOW);
      headlight = "OFF";
    }
    Serial.println("headlight");
    Serial.println(headlight);
  }

  if (carMode == mode::normal){
    digitalWrite(LED_RED, LOW);

    //String turn;
    
    if (isButtonPressed(BTN_LEFT, lastBtnLeftState)) {
    goLeft = true;
    goRight = false;
    turn = "Turn Left";
    Serial.println("turn");
    Serial.println(turn);
  } else if (isButtonPressed(BTN_RIGHT, lastBtnRightState)) {
    goLeft = false;
    goRight = true;
    turn = "Turn Right";
    Serial.println("turn");
    Serial.println(turn);
  }
  
  int steering = map(analogRead(POT), 0, 1024, 0, 2);

  if (millis() - lastTurn > TURN_DELAY) {
    lastTurn = millis();

    if (steering == Steering::left) {
      if (oldSteering == Steering::right) {
        goRight = false;
        digitalWrite(LED_BLUE, LOW);
      }
    } else if (steering == Steering::right) {
      if (oldSteering == Steering::left) {
        goLeft = false;
        digitalWrite(LED_YELLOW, LOW);
      }
    }
    oldSteering = steering;
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
    
    ledState = ! ledState;
   }
  }

  if (carMode == mode::hazard){
    if (millis() - lastTimeBlinked > LED_INTERVAL) {
      lastTimeBlinked = millis();
      digitalWrite(LED_YELLOW, ledState);
      digitalWrite(LED_BLUE, ledState);
      digitalWrite(LED_RED, ledState);
    }
    
    ledState = ! ledState;

    if (isButtonPressed(BTN_LEFT, lastBtnLeftState)) {
    goLeft = true;
    goRight = false;
    turn = "Turn Left";
    Serial.println("turn");
    Serial.println(turn);
  } else if (isButtonPressed(BTN_RIGHT, lastBtnRightState)) {
    goLeft = false;
    goRight = true;
    turn = "Turn Right";
    Serial.println("turn");
    Serial.println(turn);
  }
  
  int steering = map(analogRead(POT), 0, 1024, 0, 2);

  if (millis() - lastTurn > TURN_DELAY) {
    lastTurn = millis();

    if (steering == Steering::left) {
      if (oldSteering == Steering::right) {
        goRight = false;

      }
    } else if (steering == Steering::right) {
      if (oldSteering == Steering::left) {
        goLeft = false;

      }
    }
    oldSteering = steering;
  }
 }

  

  if (Serial.available()){
    String command = Serial.readStringUntil('\n');
    if (command == "alarm"){
      carMode = mode::hazard;
      digitalWrite(LED_RED, HIGH);
    }
    if (command == "noalarm") {
      carMode = mode::normal;
    }
  }
}

float get_temperature(){
  float temperature, resistance;
  int value;
  value = analogRead(NTC);                      //Temperature app
  resistance   = (float)value * NTC_R25 / (1024 - value); // Calculate resistance
  temperature  = 1 / (log(resistance / NTC_R25) / NTC_MATERIAL_CONSTANT + 1 / 298.15) - 273.15;
  return temperature;
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
