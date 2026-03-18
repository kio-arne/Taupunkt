// initialize WIFI
void setupWifi() {
  // WIFI credentials
  const char* ssid     = "B13";
  const char* password = "g0llh0fen";

  //connect
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
   // yield();
    Serial.print(".");
    delay(200);
  }

  // Print local IP address
  Serial.print("IP-Addresse: ");
  Serial.println(WiFi.localIP());
}

////////////////////////////////////////////////////////////////////////////////
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    yield();
    // Create a random client ID
    String clientId = "ESP32-WROOM-Luftsteuerung";
    //          clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("WLAN Ok! \n");
    } else {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println("Neustart des WLAN in 5 Sekunden");
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}
