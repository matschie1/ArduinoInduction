class OneWireSensor
{    
  const char* script_prefix = "/api/httpsensor/"; 
  unsigned long lastCalled;
  long period = 5000;

  public:
  char script_sensor[50];
  byte index_sensor;  
  String name_sensor;
  
  OneWireSensor(byte index, String script, String sname) { 
    script.toCharArray(script_sensor,script.length()+1);
    index_sensor = index;
    name_sensor = sname;
    }
  float Value;
  
 
  void Update() {
    if (millis() > (lastCalled + period)) {  
      DS18B20.requestTemperatures();
      Value = DS18B20.getTempCByIndex(index_sensor);

      publishmqtt(); 

      lastCalled = millis();
      Serial.println(script_sensor);   
     }     
  }

  void change(int index, String script, String sname) {
    index_sensor = index;
    script.toCharArray(script_sensor,script.length()+1);
    name_sensor = sname;
  }

  void publishmqtt() {
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["Name"] = name_sensor;
    JsonObject& Sensor = json.createNestedObject("Sensor");
    Sensor["Value"] = Value;
    Sensor["Type"] = "1-wire";

    char jsonMessage[100];
    json.printTo(jsonMessage);
    client.publish(script_sensor,jsonMessage);
  }
  
  char* getValueString() {
   char buf[5];
   dtostrf(Value,2,1,buf);
   return buf;
   }
  
};


OneWireSensor sensors[10] = {
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"",""),
  OneWireSensor(0,"","")
  };
int numberOfSensors;   
  

void handleSensors() {
    for (int i = 0; i < numberOfSensors; i++) {
      sensors[i].Update();
      yield();
  }
}

void handleRequestSensors() {
  String message;
  for (int i = 0; i < numberOfSensors; i++) {
    message += F("<li class=\"list-group-item d-flex justify-content-between align-items-center\">");
    message += sensors[i].name_sensor;
    message += F("<span class=\"badge badge-light\">");
    message += sensors[i].getValueString(); 
    message += F("&deg;C</span><span class=\"badge badge-info\">");    
    message += sensors[i].script_sensor;
    message += F("</span> <a href=\"\" class=\"badge badge-warning\" data-toggle=\"modal\" data-target=\"#sensor_modal\" data-value=\"");
    message += i;
    message += F("\">Edit</a> </li>");  
    yield();
  }
  message += F("<li class=\"list-group-item justify-content-between align-items-center d-flex flex-row-reverse\"> <a href=\"\" class=\"badge badge-warning\" data-toggle=\"modal\" data-target=\"#sensor_modal\" data-value=\"-1\">Add</a></li>");
  server.send(200,"text/html", message);
}

void handleRequestSensor() {
  int id = server.arg(0).toInt();
  String request = server.arg(1);
  String message;

  if (id == -1) {
    message = "";
    goto SendMessage;  
  } else {
    if (request == "name") { message = sensors[id].name_sensor; goto SendMessage; }
    if (request == "script") { message = sensors[id].script_sensor; goto SendMessage; }
    message = "not found";    
  }
  saveConfig();
  SendMessage:
    server.send(200,"text/plain",message); 
}

void handleSetSensor() {
  int id = server.arg(0).toInt();
  
  if (id == -1) {
    id = numberOfSensors;
    numberOfSensors += 1;            
  }

  String sens_script = sensors[id].script_sensor;
  String sens_name = sensors[id].name_sensor;


  for (int i = 0; i < server.args(); i++) {
    
    if (server.argName(i) == "name") { sens_name = server.arg(i); }  
    if (server.argName(i) == "script")  { sens_script = server.arg(i); }
    yield();       
  }

  sensors[id].change(id,sens_script,sens_name);
  
  saveConfig();
}

void handleDelSensor() {
  int id = server.arg(0).toInt();

   for (int i = id; i < numberOfSensors; i++) {
    if (i == 9) { sensors[i].change(0,"",""); } else {
      sensors[i].change(sensors[i+1].index_sensor,sensors[i+1].script_sensor,sensors[i+1].name_sensor);     
    }
   }
  
  numberOfSensors -= 1;
  saveConfig();
}
 
unsigned long lastPublished;

void publishStatus() {
  if (millis() > (lastPublished + 1000)) { 
    lastPublished = millis();

    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["Name"] = "inductionsensor";
    JsonObject& Sensor = json.createNestedObject("Sensor");
    if (isPower) {
      Sensor["Value"] = CMD_CUR;
    } else {
      Sensor["Value"] = max(0,CMD_CUR-1);
    }
    Sensor["Type"] = "1-wire";

    char jsonMessage[100];
    json.printTo(jsonMessage);
    client.publish("indusens",jsonMessage);
    
  }
  }
