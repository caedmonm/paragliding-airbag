
#include <Arduino.h>
#include <Wire.h>
#include <TFLI2C.h>

// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27, 20, 4);

TFLI2C tflI2C;

int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
float distance;
float distanceChange;
float closingSpeed;
float timeToImpact;
int reportLive = 12;
int reportIncident = 13;
int reportDeploy = 15;

// LOOP PERIOD VARS
const unsigned long loopPeriod = 100;
float timeSinceLast;
unsigned long lastLoop;
unsigned long lastReading;

// historic data
const int recordForSeconds = 5;
const int historicDataArraySize = recordForSeconds * (1000/loopPeriod);
float closingSpeeds[historicDataArraySize];
float distances[historicDataArraySize];
// INCIDENT TRIGGER VARS
bool incident = false;
int triggerMinDist = 20;  // minimum distance to START incident
int triggerCS = 200;      // closing speed to trigger incident (cm/s)
float standDownCheckPeriod = 2000; // period to look back over before deploying (in ms)
int standDownMaxCS = 100; // cm/s to standdown

// MONITOR VARS
bool deploy = false;
float deployCheckPeriod = 1000; // period to look back over before deploying (in ms)
int deployCS = triggerCS; // might vary later but for now it's same as trigger

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(0, OUTPUT);
  pinMode(reportLive, OUTPUT);
  pinMode(reportIncident, OUTPUT);
  pinMode(reportDeploy, OUTPUT);

  // lcd.init();
  // lcd.clear();
  // lcd.backlight();
}

void loop() {
  if (tflI2C.getData(tfDist, tfAddr)) {
    timeSinceLast = millis() - lastReading;
    lastReading = millis();

    // Measurements
    distanceChange = distance - tfDist;
    distance = tfDist;
    closingSpeed = distanceChange / (timeSinceLast / 1000);
    timeToImpact = distance / closingSpeed;

    // Shuffle along values
    for (int i = historicDataArraySize - 1; i >= 1; i--) {
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

    if (incident) {
      monitorIncident();
    }

    int reportPin = reportLive;
    if (deploy) {
      reportPin = reportDeploy;
    } else if (incident) {
      reportPin = reportIncident;
    }
    digitalWrite(reportPin, HIGH);
    delay(10);
    digitalWrite(reportPin, LOW);
  }

  // Stabilize frequency to loopPeriod (100ms)
  while (millis() - lastLoop < loopPeriod) {
    // Wait for the loop period to complete
  }
  lastLoop = millis();
}
