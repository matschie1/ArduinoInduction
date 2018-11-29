void setup() {
  Serial.begin(9600);

  lcd.init();

  // Relais
  pinMode(PIN_WHITE, OUTPUT);
  digitalWrite(PIN_WHITE, LOW);

  // Rückmeldung der Platte
  pinMode(PIN_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), readInput, CHANGE);

  // Meldung an Platte
  pinMode(PIN_YELLOW, OUTPUT);
  digitalWrite(PIN_YELLOW, HIGH);

  setupCommands();    // Ersetzt "1" durch "Signal_high" und "0" durch "Signal_low"
}

void setupCommands() {
  for (int i = 0; i < 33; i++) {
    for (int j = 0; j < 6; j++) {
      if    ( CMD[j][i] == 1)  { CMD[j][i] = SIGNAL_HIGH; }
      else                     { CMD[j][i] = SIGNAL_LOW;  }
    }   
  }
}
