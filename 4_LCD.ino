void updateLCD() {
  if (isInduon) {
    lcd.backlight();
    lcdPrintMenu();
  } else {
    if (millis() > timeLastLCD + timeOutLCD) {
        lcd.noBacklight();      
    }    
  }
}

void lcdPrintMenu() {
  lcd.setCursor(0,0); 
  lcd.print("M:SER");
}
void lcdPrintPower() {
  // Power in %
  lcd.setCursor(8,0);
  if ( power < 100 ) { lcd.print(" "); }
  if ( power < 10 )  { lcd.print(" "); }
  lcd.print(power);
  lcd.print("%");
}

void lcdPrintPowerLevel() {
  // Derzeitige Stufe
  lcd.setCursor(14,0);
  lcd.print("P");    
  lcd.setCursor(15,0);
  if ( isPower != true && power > 0) { lcd.print(CMD_CUR-1); } 
  else  { lcd.print(CMD_CUR); }
}
 
void lcdPrintError() {
  lcd.setCursor(0,1);
  lcd.print(errorMessage);
  Serial.println(errorMessage);  
  timeLastLCD = millis();   
}     
