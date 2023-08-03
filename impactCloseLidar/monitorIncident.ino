void monitorIncident() {
  if (closingSpeed < canIncCS) {
    incident = false;
    Serial.println("Stand down");
  } else {
    if (distance < 200) {
      Serial.println("DEPLOY");
      deploy = true;
    }
  }
}