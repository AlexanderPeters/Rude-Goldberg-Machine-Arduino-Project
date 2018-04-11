//Author: Alexander Peters
//Code Written On 4/7/18
//Code Purpose: This code was quickly hacked together to help a friend
// with a project for a science olympiad competition. (Rude Goldberg machine)
// Code Utilized by: Prajwal Jagadish

//Libraries
#include <Servo.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"

//Pins and object
int lightSensor = 2;
int limitSwitch = 39;
int limitSwitch2 = 36;
int breakBeam = 25;
int relay = 14;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Servo servo_3;
Servo servo_4;
Servo servo_5;
Servo servo_7;

//Lockouts
boolean servo_3_lockout = false;
boolean servo_4_lockout = false;
boolean servo_5_lockout = false;
boolean servo_7_lockout = false;
boolean relay_lockout = false;

//Servo Pins
int pos_servo_3 = 0;
int speed_servo_4 = 0;
int speed_servo_5 = 0;
int pos_servo_7 = 0;

//Servo Vars
boolean runServo_4 = true;
boolean runServo_5 = true;
int speedOff = 90;
int speedVal = 120;
unsigned long startingMillis1 = 0;
unsigned long currentMillis1;
unsigned long runMillis1 = 15000;
unsigned long startingMillis2 = 0;
unsigned long currentMillis2;
unsigned long runMillis2 = 15000;

//Light Sensor Vars
float rawRange = 1024; // 3.3v
float logRange = 5.0; // 3.3v = 10^5 lux

//Light Sensor Trigger
float lightSensorTrigger = 350; // Flux

//Temp Sensor Trigger
float tempSensorTrigger = 50; // Deg. F

//Method for converting Raw Analog Values into Flux units
float RawToLux(int raw) {
  float logLux = raw * logRange / rawRange;
  return pow(10, logLux);
}


void setup() {
  //Set Pin Modes
  pinMode(lightSensor, INPUT);
  pinMode(limitSwitch, INPUT);
  pinMode(limitSwitch2, INPUT);
  pinMode(breakBeam, INPUT);
  pinMode(relay, OUTPUT);
  
  //Attach Relays
  servo_3.attach(3);
  servo_4.attach(4);
  servo_5.attach(5);
  servo_7.attach(7);
  
  //Default Servo Positions and Speeds
  servo_3.write(0);
  servo_4.write(speedOff);    
  servo_5.write(speedOff);
  servo_7.write(0); 

  //Analog Reference
  analogReference(EXTERNAL);

  //Init Serial
  Serial.begin(9600);
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

void loop() {
  //Read in Temp Sensor
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;

  //Read in Light Sensor
  int lightSensorRawValue = analogRead(lightSensor);    

  //Check Temp Sensor
  if(f <= tempSensorTrigger && !servo_7_lockout) {
    for (pos_servo_7 = 0; pos_servo_7 <= 210; pos_servo_7 += 1) {
      servo_7.write(pos_servo_7);              
      delay(15);                     
    }
    servo_7_lockout = true;
  }
  
  //Check Light Sensor
  float sensorLux = RawToLux(lightSensorRawValue);
  if(sensorLux >= lightSensorTrigger && !servo_5_lockout) {
      currentMillis2 = millis();
      if (runServo_5 == true) {
        if (currentMillis2 - startingMillis2 >= runMillis2) {// Time has expired
          servo_5.write(speedOff);
          runServo_5 = false;
        }
        else {// Time has not expired
          servo_5.write(speedVal);
        }
      }
    servo_5_lockout = true;
  }
  
  //Check Break Beam
  int beamState = digitalRead(breakBeam);
  if(beamState == LOW && !servo_4_lockout) {
      currentMillis1 = millis();
      if (runServo_4 == true) {
        if (currentMillis1 - startingMillis1 >= runMillis1) {// Time has expired
          servo_4.write(speedOff);
          runServo_4 = false;
        }
        else {// Time has not expired
          servo_4.write(speedVal);
        }
      }
    servo_4_lockout = true;
  }
  
  //Check limit switch
  if(limitSwitch == false && !servo_3_lockout) {
    for (pos_servo_3 = 0; pos_servo_3 <= 210; pos_servo_3 += 1) {
      servo_3.write(pos_servo_3);              
      delay(15);                     
    }
    servo_3_lockout = true;
  }
  
  //Check second limit switch
  if(limitSwitch2 == false && !relay_lockout) {
    digitalWrite(relay, HIGH);
    relay_lockout = true;
  }    
}
