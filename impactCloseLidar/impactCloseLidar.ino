#include <Arduino.h>
#include <Wire.h>
#include <TFLI2C.h>
#include <LiquidCrystal_I2C.h>

TFLI2C tflI2C;

int16_t tfDist;
int16_t tfAddr = TFL_DEF_ADR;
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int buzzerPin = 5;
float distance;
float distanceChange;
float closingSpeed;
float timeToImpact;

const int arraySize = 20;
float closingSpeeds[arraySize];
float distances[arraySize];

const unsigned long loopPeriod = 100;
float timeSinceLast;
unsigned long lastLoop;
unsigned long lastReading;

bool incident = false;
bool inflate = false;

int triggerMinDist = 200; // minimum distance to START incident
int triggerMinCS = 2; // closing distance to trigger incident

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

    distanceChange = distance - tfDist;
    distance = tfDist;
    closingSpeed = distanceChange / (timeSinceLast/1000);
    timeToImpact = distance / closingSpeed;

    // Call the function before adding new values
    for (int i = arraySize - 1; i >= 1; i--) {
      closingSpeeds[i] = closingSpeeds[i - 1];
      distances[i] = distances[i - 1];
    }

    // Add new data to the arrays
    closingSpeeds[0] = closingSpeed;
    distances[0] = distance;

    // SCREEN OUTPUT
    lcd.setCursor(0, 0);
    lcd.print(String(tfDist) + " cm        ");
    lcd.setCursor(0, 1);
    lcd.print(String(closingSpeed) + " cm/s      ");
    lcd.setCursor(0, 2);
    lcd.print(String(timeToImpact) + " s until impact    ");

    // SERIAL OUTPUT
    // Serial.println(String(timeSinceLast/1000)+" seconds since last reading");
    // Serial.println(String(tfDist) + " cm");
    // Serial.println(String(closingSpeed) + " cm/s");
    Serial.println("TtI:" + String(timeToImpact) + "s");

    // TRIGGER CHECK
    if (!incident) {
      if(tfDist > triggerMinDist && closingSpeed > triggerMinCS){
        incident = true;
      }

    } else {

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

  // Serial.print("c: ");
  // for (int i = 0; i < arraySize; i++) {
  //   Serial.print(closingSpeeds[i]);
  //   Serial.print(", ");
  // }
  // Serial.println("");
}
