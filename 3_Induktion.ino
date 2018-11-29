/*--- Update Funktionen ---*/
bool updateRelay() {
  if (isInduon == true && isRelayon == false) {         /* Relais einschalten */
    digitalWrite(PIN_WHITE, HIGH);      
    return true;
  } 

  if (isInduon == false && isRelayon == true) {         /* Relais ausschalten */
    if (millis() > timeTurnedoff + delayAfteroff) {
       digitalWrite(PIN_WHITE, LOW);      
       return false;
    } 
  }  

  if (isInduon == false && isRelayon == false) {        /* Ist aus, bleibt aus. */
  return false;
  }

  return true;                                          /* Ist an, bleibt an. */
}


void updatePower() {
  if (power != newPower) {                              /* Neuer Befehl empfangen */
    
    if (newPower > 100) { newPower = 100; }             /* Nicht > 100 */
    if (newPower < 0)   { newPower = 0;   }             /* Nicht < 0 */
    
    power = newPower;
    lcdPrintPower();
    
    timeTurnedoff = 0;
    isInduon = true;        
    long difference = 0;    
    
    if (power == 0) {
      CMD_CUR = 0;
      timeTurnedoff = millis();
      isInduon = false;
      difference = 0;
      goto setPowerLevel;
    }

    for (int i = 1; i < 7; i++) {
      if (power <= PWR_STEPS[i]) {
        CMD_CUR = i;
        difference = PWR_STEPS[i] - power;        
        goto setPowerLevel;
      }      
    }

    setPowerLevel:                                      /* Wie lange "HIGH" oder "LOW" */
      if (difference != 0) {
        powerLow = powerSampletime * difference / 20L;
        powerHigh = powerSampletime - powerLow;
      } else { 
        powerHigh = powerSampletime; 
        powerLow = 0;
      };          
  }
  lcdPrintPower();
  lcdPrintPowerLevel();
}

void updateCommand() {
  if (isRelayon == true && isInduon == true) {

    unsigned long timeNow = millis();                 /* aktuelle millis() festhalten */
    
    if (isPower == true) {                            /* Aktuell Hohe Stufe */                         
      if (timeNow > powerLast + powerHigh) {          /* Prüfen, ob Zeit für Hohe Stufe vorbei */
        isPower = false;                              /* Wenn ja, Niedrige Stufe. */
        lcdPrintPowerLevel();
        powerLast = millis();                         /* Zeit festhalten. */
      } else {  
        sendCommand(CMD[CMD_CUR]);                    /* Befel "Hohe Stufe" sennden. */ 
      }
    } else {                                          /* Aktuell niedrige Stufe. */
      if (timeNow > powerLast + powerLow) {           /* Prüfen, ob Zeit für niedrige Stufe vorbei */
        isPower = true;                               /* Wenn ja, hohe Stufe. */
        lcdPrintPowerLevel();
        powerLast = millis();                         /* Zeit festhalten. */
      } else {
        sendCommand(CMD[(CMD_CUR - 1)]);              /* Befel "Niedrige Stufe" sennden. */     
      }
    }     
  } else {                                            
     isPower = false;
     powerLast = 0;
     sendCommand(CMD[0]);                             /* Befel "Niedrige Stufe" sennden. */
     lcdPrintPowerLevel();
  }
}


/*--- Send and Read ---*/
void sendCommand(int command[33]) {
  digitalWrite(PIN_YELLOW, HIGH);
  delay(SIGNAL_START);
  digitalWrite(PIN_YELLOW, LOW);
  delay(SIGNAL_WAIT);


  for (int i = 0; i < 33; i++) {
    digitalWrite(PIN_YELLOW, HIGH);
    delayMicroseconds(command[i]);
    digitalWrite(PIN_YELLOW, LOW);
    delayMicroseconds(SIGNAL_LOW);
  }
}

void readInput() {
  // Variablen sichern
  bool ishigh = digitalRead(PIN_INTERRUPT);
  unsigned long newInterrupt = micros();
  long signalTime = newInterrupt - lastInterrupt;

  // Glitch rausfiltern
  if (signalTime > 10) {

    if (ishigh) {
      lastInterrupt = newInterrupt;         // PIN ist auf Rising, Bit senden hat gestartet :)
    }  else {                               // Bit ist auf Falling, Bit Übertragung fertig. Auswerten.

      if (!inputStarted) {                  // suche noch nach StartBit.
        if (signalTime < 35000L && signalTime > 15000L) {
          inputStarted = true;
          inputCurrent = 0;
        }
      } else {                              // Hat Begonnen. Nehme auf.
        if (inputCurrent < 34) {            // nur bis 33 aufnehmen.
          if (signalTime < (SIGNAL_HIGH + SIGNAL_HIGH_TOL) && signalTime > (SIGNAL_HIGH - SIGNAL_HIGH_TOL) ) {
            // HIGH BIT erkannt
            inputBuffer[inputCurrent] = 1;
            inputCurrent += 1;
          }
          if (signalTime < (SIGNAL_LOW + SIGNAL_LOW_TOL) && signalTime > (SIGNAL_LOW - SIGNAL_LOW_TOL) ) {
            // LOW BIT erkannt
            inputBuffer[inputCurrent] = 0;
            inputCurrent += 1;
          }
        } else {                            // Aufnahme vorbei.
  
        /* Auswerten */
        newError = BtoI(13,4);          // Fehlercode auslesen.
          
        /* von Vorne */    
        timeLastReaction = millis();
        inputCurrent = 0;
        inputStarted = false;
        }
      }
    }
  }
}

unsigned long BtoI(int start, int numofbits){    //binary array to integer conversion
 unsigned long integer=0;
 unsigned long mask=1;
 for (int i = numofbits+start-1; i >= start; i--) {
   if (inputBuffer[i]) integer |= mask;
   mask = mask << 1;
 }
 return integer;
}    
