bool loadConfig() {
  File configFile = SPIFFS.open("/actors.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  } else { Serial.println("opened config file"); }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    return false;
  } 

//  JsonArray& jsonactors = json["actors"];

//  numberOfActors = jsonactors.size();
//  
//  if (numberOfActors > 6) { 
//    numberOfActors = 6; 
//  }
  
//  // Read Actors  
//  for (int i = 0; i < numberOfActors; i++) {
//    if (i < numberOfActors) {
//     JsonObject& jsonactor = jsonactors[i];    
//     String pin = jsonactor["PIN"];
//     String script = jsonactor["SCRIPT"];
//     String aname = jsonactor["NAME"];
//     actors[i].change(pin,script,aname);     
//    } //else {
//      //actors[i].change("","","");
//    //}
//    yield();
//  }

  JsonArray& jsonsensors = json["sensors"];
  numberOfSensors = jsonsensors.size();

  if (numberOfSensors > 10) { 
    numberOfSensors = 10; 
  }

  for (int i = 0; i < 10; i++) {
    //String script = "";
    //String aname = "";
    //String pin = "";
    if (i < numberOfSensors) {
     JsonObject& jsonsensor = jsonsensors[i];    
     int index = atoi(jsonsensor["INDEX"]);
     String ascript = jsonsensor["SCRIPT"];
     String aname = jsonsensor["NAME"];
     sensors[i].change(index,ascript,aname); 
     Serial.println(sensors[i].script_sensor);
    } else {
      sensors[i].change(0,"","");
    }
    yield();
  }  

  return true;
}

void saveConfigCallback () {
  Serial.println("Should save config");
}


bool saveConfig() {
  
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  File configFile = SPIFFS.open("/actors.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

//  // Write Actors
//  JsonArray& jsactors = json.createNestedArray("actors");
//  for (int i = 0; i < numberOfActors; i++) {
//    JsonObject& jsactor = jsactors.createNestedObject();
//    jsactor["PIN"] = actors[i].getPinStr();
//    jsactor["NAME"] = actors[i].name_actor;
//    jsactor["SCRIPT"] = actors[i].argument_actor;
//    yield();
//  }

  JsonArray& jssensors = json.createNestedArray("sensors");
  for (int i = 0; i < numberOfSensors; i++) {
    JsonObject& jssensor = jssensors.createNestedObject();
    jssensor["INDEX"] = String(i);
    jssensor["NAME"] = sensors[i].name_sensor;
    jssensor["SCRIPT"] = sensors[i].script_sensor;
    yield();
  }
  
  json.printTo(configFile);
  return true;
}
