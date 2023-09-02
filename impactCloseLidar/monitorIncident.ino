void monitorIncident() {
  // Check if it should stand down
  int numberToCheck = standDownCheckPeriod / loopPeriod;

  // To stand down ALL readings within the check period should be less than standDownMaxCS
  bool shouldStandDown = true;
  for (int i = 0; i < numberToCheck; i++) {
    if (closingSpeeds[i] > standDownMaxCS) {
      shouldStandDown = false;
    }
  }

  if (shouldStandDown) {
    incident = false;
    deploy = false;
    Serial.println("Stand down");
  } else {
    int numberToCheck = deployCheckPeriod / loopPeriod;

    // To deploy ALL readings should be > deployCS value
    bool shouldDeploy = true;
    for (int i = 0; i < numberToCheck; i++) {
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