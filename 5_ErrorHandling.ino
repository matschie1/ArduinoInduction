void watchDog() {
  /*--- Serielle kommunikation Überwachen ---*/
  if ((timeLastCommand + timeOutCommand) < millis()) { 
    isSerialError = true; 
    if (!skipSerialError){
      storePower = newPower;
      if (power != 0) { newPower = 0; }
    }
  } else { 
    isSerialError = false; 
  }

  /*--- Induktionskochfeld Überwachen ---*/
   if ((timeLastReaction + timeOutReaction) < millis()) { 
     isCommError = true; 
   } else { 
     isCommError = false; 
   }  
   
   updateError(); 
}

boolean updateError() {
  boolean returnValue = false;
  String errorMessageBuilder = "";

  // Platte meldet Okay
  if (newError == 0) { 
    resetInduErrors();    // Alle Plattenfehler zurücksetzen.
    goto SerialErrors;    // Fehler überspringen.
  } 
  
  returnValue = true;     // Nicht okay
  switch (newError) {
    case 2:
      isInduError[0] = true;
      break;
    default:
      isunknownError = true;
  }

    SerialErrors:
      if (isCommError)   { returnValue = true; errorMessageBuilder = "I"; } 
      if (isSerialError) { returnValue = true; errorMessageBuilder = errorMessageBuilder + "S"; }

    for (int i = 0; i < 10; i++) {
      if (isInduError[i]) { errorMessageBuilder = errorMessageBuilder + errorMessages[i]; }
    }

    if (isunknownError) { errorMessageBuilder = errorMessageBuilder + "F:" + newError; }

    if (returnValue == false) {
       errorMessageBuilder = "                "; 
       
    }
    if (errorMessageBuilder != errorMessage) {
      errorMessage = errorMessageBuilder;
      lcdPrintError();
    }

    return returnValue;
}

void handleError() {
  if (power != 0) {
    lcdPrintError(); 
    newPower = 0;
    updatePower();     
    }   
  };

void resetInduErrors() {
  for (int i = 0; i < 10; i++) {
    isInduError[i] = false;
  }
  }
