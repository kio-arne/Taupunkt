///////////////////////////////////////////////////////////////
// Program for SKU861168-24164072
// 
///////////////////////////////////////////////////////////////

// DATA Definition ////////////////////////////////////////////

//*************************************************************
#include <DHT.h>
#include <Wire.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SSD1306.h>

#define Humidity_i_topic "Innen/Humidity/Luft"
#define Temperatur_i_topic "Innen/Temperatur/Luft"
#define Humidity_a_topic "Außen/Humidity/Luft"
#define Temperatur_a_topic "Außen/Temperatur/Luft"
#define Sensorpause 30000 // gaps between two checks; max 3 Seconds because of the WatchDog
#define LED_shine 500 // work in progress info
#define DHTTYPE DHT22 // one type of sensor for both in- and outside
#define RELAIS_PIN 25
#define DHTPIN_Inside 26
#define DHTPIN_Outside 27
#define LED_PIN 2 // Internal LED Pin

//Display initialization
SSD1306 display(0x3C, 21, 22);

//*************************************************************
// Structure for Sensor Data
struct Measure
{
  float Temperature;
  float Humidity;
};

// set client
WiFiClient espClient;
PubSubClient client(espClient);

// sensors
DHT inside(DHTPIN_Inside, DHTTYPE);
DHT outside(DHTPIN_Outside, DHTTYPE);

////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);

  display.init();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);

  displayOut("Start");  
  Serial.println("100 Start");

  // digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  displayOut("200 LED Builtin");

  // Relais PIN for output and off
  pinMode(RELAIS_PIN, OUTPUT);
  digitalWrite(RELAIS_PIN, LOW);
  displayOut("300 Relais aus");

  // do the startup sequence
  setupWifi();
  displayOut("400 WLAN");
  setupMQTT();
  displayOut("500 MQTT");

  Serial.println("Inside");  
  pinMode(DHTPIN_Inside, INPUT);
  inside.begin();
  displayOut("600 Inside");

  Serial.println("Outside");
  pinMode(DHTPIN_Outside, INPUT);
  outside.begin();
  displayOut("700 Outside");
  Serial.println("Setup done");
  
  blink(1);
  displayOut("999 Setup done");
  delay(1000);
}


////////////////////////////////////////////////////////////////////////////////
void loop()
{
  Serial.println("Loop go");
  timeBlink(2,10);
  display.clear();
  Measure Drinnen(readSensor(inside));
  displayLine(String(Drinnen.Temperature), 2);
  Measure Draussen(readSensor(outside));
  displayLine(String(Draussen.Temperature), 3);
//TODO
  
  //*************************************************************************************
  // MQTT sending message
  if (!client.connected())
  {
    timeBlink(2,80);
    Serial.println("Reconnect");
    reconnect();
  }
//  client.loop();
//  sprintf(cstr, "%f", T); //Transfer the double to char for publishing
//  client.publish(Temperatur_a_topic, cstr); //Published Temperature

//  sprintf(cstr, "%f", P); //Transfer the double to char for publishing
//  client.publish(Humidity_a_topic, cstr); //Published Preassure

  //*************************************************************************************
  // Blink the LED and pause
  Serial.println("Sensorpause");
  delay(Sensorpause);  // Pause, wobei 1000 = 1 Sekunde
  timeBlink(1,300);
}
