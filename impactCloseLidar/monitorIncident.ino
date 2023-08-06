void monitorIncident() {
  // Check if it should stand down
  int numberToCheck = standDownCheckPeriod / loopPeriod;
  bool shouldStandDown = true;
  for (int i = 0; i < numberToCheck; i++) {
    // Serial.println("Checking SD:" + String(closingSpeeds[i]));
    if (closingSpeeds[i] > standDownMaxCS) {
      shouldStandDown = false;
    }
  }

  if (shouldStandDown) {
    incident = false;
    deploy = false;
    Serial.println("Stand down");
  } else {
    // look back over past readings to check if they're consistenly heading for impact
    int numberToCheck = deployCheckPeriod / loopPeriod;
    Serial.println("numberToCheck" + String(numberToCheck));
    bool shouldDeploy = true;
    for (int i = 0; i < numberToCheck; i++) {
      // Serial.println("Checking DP:" + String(closingSpeeds[i]));
      if (closingSpeeds[i] < deployCS) {
        shouldDeploy = false;
      }
    }
    if (shouldDeploy) {
      Serial.println("DEPLOY");
      deploy = true;
    }
  }
}