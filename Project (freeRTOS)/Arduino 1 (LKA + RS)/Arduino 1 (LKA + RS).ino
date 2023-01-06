#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <TEA5767.h>

#define enA 10  //Enable1 L298 Pin enA
#define in1 9   //Motor1  L298 Pin in1
#define in2 8   //Motor1  L298 Pin in1
#define in3 7   //Motor2  L298 Pin in1
#define in4 6   //Motor2  L298 Pin in1
#define enB 5   //Enable2 L298 Pin enB
#define red 22  //Red LED Pin 22

#define R_S A0  //ir sensor Right
#define L_S A1  //ir sensor Left

int RS_data;  // variable to receive data from the serial port

TEA5767 radio = TEA5767();

void LKA(void* pvParameters);
void RS(void* pvParameters);

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);

  xTaskCreate(RS, "RS", 128, NULL, 2, NULL);
  xTaskCreate(LKA, "LKA", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {
  // DO nothing
}

static void RS(void* pvParameters) {

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  Wire.begin();
  radio.setMuted(false);
  radio.setStereoNC(true);
  radio.setStandby(true);

  while (1) {
    //if some date is sent, reads it and saves in state
    if (Serial1.available() > 0) {
      RS_data = Serial1.read();  //if some date is sent, reads it and saves in state
      Serial.println(RS_data);
    }
    if (RS_data == 1) {      
      radio.setFrequency(90.89);
      radio.setStandby(false);
    } else if (RS_data == 2) {
      radio.setFrequency(105.30);
      radio.setStandby(false);
    } else if (RS_data == 3) {
      radio.setFrequency(105.80);
      radio.setStandby(false);
    } else if (RS_data == 4) {
      radio.setStandby(true);
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
}

static void LKA(void* pvParameters) {

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  pinMode(enA, OUTPUT);  // declare as output for L298 Pin enA
  pinMode(in1, OUTPUT);  // declare as output for L298 Pin in1
  pinMode(in2, OUTPUT);  // declare as output for L298 Pin in2
  pinMode(in3, OUTPUT);  // declare as output for L298 Pin in3
  pinMode(in4, OUTPUT);  // declare as output for L298 Pin in4
  pinMode(enB, OUTPUT);  // declare as output for L298 Pin enB

  pinMode(R_S, INPUT);  // declare ir sensor as input
  pinMode(L_S, INPUT);  // declare ir sensor as input

  pinMode(red, OUTPUT);  // declare as output for Red LED Pin 22
  
  analogWrite(enA, 125);  // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed
  analogWrite(enB, 125);  // Write The Duty Cycle 0 to 255 Enable Pin B for Motor2 Speed

  while (1) {
    //if Right Sensor and Left Sensor are at White color then it will call forward function and will turn off LED
    if ((digitalRead(R_S) == 0) && (digitalRead(L_S) == 0)) {
      forward();
      digitalWrite(red, LOW);
    }
    //if Right Sensor is Black and Left Sensor is White then it will call turn Right function and will turn on LED
    if ((digitalRead(R_S) == 1) && (digitalRead(L_S) == 0)) {
      turnRight();
      digitalWrite(red, HIGH);
    }
    //if Right Sensor is White and Left Sensor is Black then it will call turn Left function and will turn on LED
    if ((digitalRead(R_S) == 0) && (digitalRead(L_S) == 1)) {
      turnLeft();
      digitalWrite(red, HIGH);
    }
    //if Right Sensor and Left Sensor are at Black color then it will call Stop function and will turn off LED
    if ((digitalRead(R_S) == 1) && (digitalRead(L_S) == 1)) {
      Stop();
      digitalWrite(red, LOW);
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}

void forward() {            //forward
  digitalWrite(in1, HIGH);  //Right Motor forward Pin
  digitalWrite(in2, LOW);   //Right Motor backword Pin
  digitalWrite(in3, LOW);   //Left Motor backword Pin
  digitalWrite(in4, HIGH);  //Left Motor forward Pin
}

void backward() {           //backward
  digitalWrite(in1, LOW);   //Right Motor forword Pin
  digitalWrite(in2, HIGH);  //Right Motor backword Pin
  digitalWrite(in3, HIGH);  //Left Motor backword Pin
  digitalWrite(in4, LOW);   //Left Motor forword Pin
}

void turnRight() {          //turnRight
  digitalWrite(in1, LOW);   //Right Motor forward Pin
  digitalWrite(in2, HIGH);  //Right Motor backword Pin
  digitalWrite(in3, LOW);   //Left Motor backword Pin
  digitalWrite(in4, HIGH);  //Left Motor forward Pin
}

void turnLeft() {           //turnLeft
  digitalWrite(in1, HIGH);  //Right Motor forward Pin
  digitalWrite(in2, LOW);   //Right Motor backword Pin
  digitalWrite(in3, HIGH);  //Left Motor backword Pin
  digitalWrite(in4, LOW);   //Left Motor forward Pin
}

void Stop() {              //stop
  digitalWrite(in1, LOW);  //Right Motor forward Pin
  digitalWrite(in2, LOW);  //Right Motor backword Pin
  digitalWrite(in3, LOW);  //Left Motor backword Pin
  digitalWrite(in4, LOW);  //Left Motor forward Pin
}