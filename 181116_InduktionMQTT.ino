/*
 * Sketch zur Kommunikation zwischen einem CBPi und einem GGM Induktionskochfeld.
 * v.0.3 vom 16.11.2018
 * Kommunikation über WiFi via MQTT - Basis ESP8266
 * M.Sc.
 * 
 * Benutzung auf eigene gefahr.
 * 
 */
 
/*############## INCLUDES ###################*/
#include <Wire.h>                 // i2C
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>             
#include <FS.h>                           
#include <ArduinoJson.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include "config.h"               // Config-Datei
#include <PubSubClient.h>
/*############## KONSTANTEN ##################*/

#define ONE_WIRE_BUS D3

ESP8266WebServer server(80);

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);


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

/*----- WiFi ----*/
char indutopic[16] = "indu";
char mqtt_server[16] = "192.168.178.236";

IPAddress ip;
String errorMessage    =  "";     // Nachricht für Display 

/*-----  CBPi  -----*/
unsigned long timeLastCommand;    // Letzer empfangener Command von CBPi


/*############## PIN-Zuordnung ##################*/

/*-----  Induktionskochfeld  -----*/
const byte PIN_WHITE = D5;       // RELAIS
const byte PIN_YELLOW = D7;      // AUSGABE AN PLATTE
const byte PIN_INTERRUPT = D6;   // EINGABE VON PLATTE
