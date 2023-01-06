#include <Arduino_FreeRTOS.h>

int xValue = 0;
int yValue = 0;
int bValue = 0;
bool driveFlag = false;
bool reverseFlag = false;
bool parkFlag = false;
bool neutralFlag = false;

byte pin[] = { 2, 3, 4, 5, 6, 7, 8, 9 };  //arduino pin array

//gear array
int gear[5][8] = {

  { 0, 1, 1, 1, 0, 0, 0, 0 },  //P
  { 0, 0, 0, 1, 0, 0, 0, 1 },  //D
  { 0, 1, 0, 1, 0, 0, 0, 1 },  //N
  { 0, 1, 1, 1, 0, 0, 0, 1 },  //R
  { 1, 1, 1, 1, 1, 1, 1, 1 },  //OFF
};

const int ldrPin = A15;
const int ledPin1 = 39;
const int ledPin2 = 38;

int ldrValue = 0;

void Gear(void* pvParameters);
void Headlights(void* pvParameters);

void setup() {

  Serial.begin(9600);

  xTaskCreate(Gear, "Gear", 128, NULL, 2, NULL);
  xTaskCreate(Headlights, "Headlights", 128, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {
  // DO nothing
}

static void Gear(void* pvParameters) {

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  pinMode(33, INPUT);
  digitalWrite(33, HIGH);
  for (byte a = 0; a < 8; a++) {
    pinMode(pin[a], OUTPUT);  //define output pins
  }

  while (1) {

    xValue = analogRead(A0);
    yValue = analogRead(A1);
    bValue = digitalRead(33);

    if (xValue >= 900 && yValue >= 400 && yValue <= 600) {  //opposite of pins
      driveFlag = true;
      reverseFlag = false;
      parkFlag = false;
      neutralFlag = false;
    } else if (xValue <= 200 && yValue >= 400 && yValue <= 600) {  //towards pins
      driveFlag = false;
      reverseFlag = true;
      parkFlag = false;
      neutralFlag = false;
    } else if (yValue >= 900 && xValue >= 400 && xValue <= 600) {  //right of pins
      driveFlag = false;
      reverseFlag = false;
      parkFlag = true;
      neutralFlag = false;
    } else if (yValue <= 200 && xValue >= 400 && xValue <= 600) {  //left of pins
      driveFlag = false;
      reverseFlag = false;
      parkFlag = false;
      neutralFlag = true;
    }
    if (driveFlag) {
      for (int b = 0; b < 9; b++) {
        digitalWrite(pin[b], gear[1][b]);  //display D
      }
    } else if (reverseFlag) {
      for (int b = 0; b < 9; b++) {
        digitalWrite(pin[b], gear[3][b]);  //display R
      }
    } else if (parkFlag) {
      for (int b = 0; b < 9; b++) {
        digitalWrite(pin[b], gear[0][b]);  //display P
      }
    } else if (neutralFlag) {
      for (int b = 0; b < 9; b++) {
        digitalWrite(pin[b], gear[2][b]);  //display N
      }
    } else {
      for (int b = 0; b < 9; b++) {
        digitalWrite(pin[b], gear[4][b]);  //display OFF
      }
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
}

static void Headlights(void* pvParameters) {

  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  while (1) {

    ldrValue = analogRead(ldrPin);
    Serial.println(ldrValue);
    if (ldrValue > 600 && ldrValue < 750) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, LOW);
    } else if (ldrValue > 750) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
    } else {
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
  }
}