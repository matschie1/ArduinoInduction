void setup() {
  Serial.begin(115200);

  // Relais
  pinMode(PIN_WHITE, OUTPUT);
  digitalWrite(PIN_WHITE, LOW);

  // Rückmeldung der Platte
  pinMode(PIN_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), readInput, CHANGE);

  // Meldung an Platte
  pinMode(PIN_YELLOW, OUTPUT);
  digitalWrite(PIN_YELLOW, HIGH);

  if (!SPIFFS.begin())  { delay(2000); } 

  loadConfig();
  
  WiFiManagerParameter cstm_mqttserver("Server", "MQTT Server", mqtt_server, 15);
  WiFiManagerParameter cstm_indutopic("Topic", "MQTT Topic für Induktion", indutopic, 15);
  
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&cstm_mqttserver);
  wifiManager.addParameter(&cstm_indutopic);

  wifiManager.autoConnect("MQTTInduktion");
  
  strcpy(mqtt_server, cstm_mqttserver.getValue());
  strcpy(indutopic, cstm_indutopic.getValue());

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttcallback);

  setupServer();  
    
    
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

void setupServer() {
  server.on("/",handleRoot);

  server.on("/setupSensor",handleSetSensor);    // Einstellen der Sensoren

  server.on("/reqSensors",handleRequestSensors);
  server.on("/reqSensor",handleRequestSensor);
  server.on("/setSensor",handleSetSensor);
  server.on("/delSensor",handleDelSensor);
  
  server.on("/reqIndu",handleRequestInduction);
  
  server.onNotFound(handleWebRequests);         // Sonstiges
  server.begin();
}

void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) { return; }
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {}

  dataFile.close();
  return true;
}

void handleRoot(){
  server.sendHeader("Location", "/blank.html",true);   //Redirect to our html web page
  server.send(302, "text/plane","");
}
