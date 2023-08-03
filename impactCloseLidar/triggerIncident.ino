bool triggerIncident() {
  if (distance > triggerMinDist && closingSpeed > triggerMinCS) {
    return true;
  }
  return false;
}