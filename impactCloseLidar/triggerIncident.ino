bool triggerIncident() {
  if (distance > triggerMinDist && closingSpeed > triggerCS) {
    Serial.println("INCIDENT!");
    Serial.println(String(closingSpeed) + " " + String(distance));
    return true;
  }
  return false;
}