Measure readSensor(DHT sensor)
{
  Measure returnValue;
  Serial.println("Sensor");
  returnValue.Temperature = sensor.readTemperature();
  returnValue.Humidity = sensor.readHumidity();
  Serial.println(String(returnValue.Temperature));
  return returnValue;
}
