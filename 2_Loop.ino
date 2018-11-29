void loop() {
  /*--- Befehl von CBPi empfangen ---*/
  while(Serial.available() > 0) {
    newPower = Serial.parseInt();
    timeLastCommand = millis();
    timeLastReaction = millis();
  }

  watchDog();

  /*--- Befehl auswerten ---*/
  updatePower();

  /*--- Relais an oder ausschalten ---*/
  isRelayon = updateRelay();  

  /*--- Befehl senden ---*/
  updateCommand();

  /*--- LCD Timeout ---*/
  updateLCD();
}
