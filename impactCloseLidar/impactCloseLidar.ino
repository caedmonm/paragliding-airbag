#include <Arduino.h>
#include <Wire.h>
#include <TFLI2C.h>
#include <LiquidCrystal_I2C.h>

TFLI2C tflI2C;

int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
LiquidCrystal_I2C lcd(0x27, 20, 4);
const int buzzerPin = 5;
float distance;
float distanceChange;
float closingSpeed;
float timeToImpact;

const int arraySize = 20;
float closingSpeeds[arraySize];
float distances[arraySize];

const unsigned long loopPeriod = 500;
float timeSinceLast;
unsigned long lastLoop;
unsigned long lastReading;

bool incident = false;
bool inflate = false;

int triggerMinDist = 20; // minimum distance to START incident
int triggerMinCS = 200; // closing speed to trigger incident (cm/s)

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop() {
  if (tflI2C.getData(tfDist, tfAddr)) {
    timeSinceLast = millis() - lastReading;
    lastReading = millis();

    // Measurements
    distanceChange = distance - tfDist;
    distance = tfDist;
    closingSpeed = distanceChange / (timeSinceLast/1000);
    timeToImpact = distance / closingSpeed;

    // Shuffle along values
    for (int i = arraySize - 1; i >= 1; i--) {
      closingSpeeds[i] = closingSpeeds[i - 1];
      distances[i] = distances[i - 1];
    }

    // Add new data to the arrays
    closingSpeeds[0] = closingSpeed;
    distances[0] = distance;

    // SCREEN OUTPUT
    LCDOutput();
    // SERIAL OUTPUT
    serialOutput();

    // TRIGGER CHECK
    if (!incident) {
      incident = triggerIncident();
    } 
    
    if(incident) {
      if (inflate && tfDist < 200 && closingSpeed > 2) {
        tone(buzzerPin, 500);
      } else {
        noTone(buzzerPin);
      }
    }
  }

  // Stabilize frequency to loopPeriod (100ms)
  while (millis() - lastLoop < loopPeriod) {
    // Wait for the loop period to complete
  }
  lastLoop = millis();

}
