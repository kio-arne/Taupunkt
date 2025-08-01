void setupMQTT() 
{
  // MQTT Broker
  const char* mqtt_server = "192.168.162.44";

  //Initialize MQTT
  Serial.print("MQTT-server: ");
  Serial.println(mqtt_server);
  Serial.print("MQTT-Topics: ");
  Serial.print(Humidity_a_topic);
  Serial.print(" | ");
  Serial.print(Temperatur_a_topic);
  Serial.println(); Serial.println(); //Empty Line
  client.setServer(mqtt_server, 1883);
}
