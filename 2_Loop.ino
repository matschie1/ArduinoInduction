void loop() {
  if (WiFi.status() != WL_CONNECTED) {  wifiManager.autoConnect("HTTPActSens"); }

  if (!client.connected()) { mqttreconnect(); }
  
  client.loop();
  
  server.handleClient();

  watchDog();

  /*--- Befehl auswerten ---*/
  updatePower();

  /*--- Relais an oder ausschalten ---*/
  isRelayon = updateRelay();  

  /*--- Befehl senden ---*/
  updateCommand();

  handleSensors();

  publishStatus();
}
