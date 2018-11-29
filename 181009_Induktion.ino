/*
 * Sketch zur Kommunikation zwischen einem CBPi und einem GGM Induktionskochfeld.
 * v.0.2 vom 09.10.2018
 * M.Sc.
 * 
 * Benutzung auf eigene gefahr.
 * 
 */
 
/*############## INCLUDES ###################*/
#include <Wire.h>                 // i2C
#include <LiquidCrystal_I2C.h>    // i2C Display
#include "config.h"               // Config-Datei

/*############## KONSTANTEN ##################*/

LiquidCrystal_I2C lcd(0x27,20,4); // Instanz Display

/*############## Variablen ##################*/

/*-----  Induktionskochfeld  -----*/
byte CMD_CUR = 0;                 // Aktueller Befehl
boolean isRelayon = false;        // Systemstatus: ist das Relais in der Platte an?
boolean isInduon = false;         // Systemstatus: ist Power > 0?
      
boolean isError = false;   
boolean isInduError[] = { false,  // E0: Kein Topf
                          false,  // E1: Stromkreisfehler
                          false,  // E2: ??
                          false,  // E3: Überhitzung 
                          false,  // E4: Temperatursensor Feheler
                          false,  // E5: ??         
                          false,  // E6: ??          
                          false,  // E7: Niederspannung
                          false,  // E8: Überspannung
                          false }; // EC: Kommunikationsfehler                                                           
boolean isunknownError =  false;  // Unbekannter Fehler
boolean isSerialError  =  false;  // Fehler in Serieller Kommunikation
boolean skipSerialError = false;  // Serial Error Überschreiben.
boolean isCommError    =  false;  // Fehler in Kommunikation mit Induktionskochfeld
int newError;

byte inputBuffer[33];             // Buffer für Rückantwort der Platte
byte inputCurrent = 0;            // Hilfvariable für Rückantwort (Zählvariable)
bool inputStarted = false;        // Starbit
unsigned long lastInterrupt;      // Zeitmessung
unsigned long timeLastReaction;   // Timeout Induktionskochfeld

unsigned long timeTurnedoff;      // Für Messung Verzögerung Abschaltung Relais

int power = 0;                    // Prozent Power Aktuell
int newPower = 0;                 // Prozent Power Neu
int storePower = 0;               // letzten Power Wert Speichern (Für ErrorHandling)
long powerSampletime = 20000;     // Dauer eines Schaltzyklus
long powerHigh = powerSampletime; // Dauer des "HIGH"-Anteils im Schaltzyklus
long powerLow = 0;                // Dauer des "LOW"-Anteils im Schaltzyklus
bool isPower = false;             // High oder Low Senden
unsigned long powerLast;          // Zeitmessung für High oder Low

/*-----  LCD  -----*/
String errorMessage    =  "";     // Nachricht für Display 
unsigned long timeLastLCD;        // LCD Timeout

/*-----  CBPi  -----*/
unsigned long timeLastCommand;    // Letzer empfangener Command von CBPi


/*############## PIN-Zuordnung ##################*/

/*-----  Induktionskochfeld  -----*/
const byte PIN_WHITE = 3;       // RELAIS
const byte PIN_YELLOW = 5;      // AUSGABE AN PLATTE
const byte PIN_INTERRUPT = 2;   // EINGABE VON PLATTE
