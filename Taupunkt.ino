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

#define Humidity_i_topic "TP/Innen/Humidity"
#define Temperatur_i_topic "TP/Innen/Temperatur"
#define Humidity_a_topic "TP/Außen/Humidity"
#define Temperatur_a_topic "TP/Außen/Temperatur"
#define Topic_Luft "TP/Luefterstart"

#define Sensorpause 58 // gaps between two checks; max 58 Seconds
#define LED_shine 500 // work in progress info
#define DHTTYPE DHT22 // one type of sensor for both in- and outside

#define RELAIS_PIN 18 //Relais HIGH=off LOW=on
#define DHTPIN_Inside 26 //Data-Line Inside
#define DHTPIN_Outside 25 //Data-Line Outside
#define LED_PIN 2 // Internal LED Pin
#define BUTTON_PIN 19 //manual Push button

//Display initialization
SSD1306 display(0x3C, 21, 22); //128x64 Pixel

//*************************************************************

// Structure for Sensor Data
struct Measure
{
  float Temperature;
  float Humidity;
};
Measure Drinnen;
Measure Draussen;

// Transformation
char cstr[8];

// Taupunkt-Differenz
float difference;

// set client
WiFiClient espClient;
PubSubClient client(espClient);

// sensors
DHT inside(DHTPIN_Inside, DHTTYPE);
DHT outside(DHTPIN_Outside, DHTTYPE);

// Runtime 
int Counter;

// Button status
volatile bool Button = LOW; //=off

// Function for a pushed Button
void ISR_pushed()             //IRAM_ATTR 
{
  Button = !Button; //switch on<>off
}

////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);

  display.init();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.flipScreenVertically();
  delay(200);
  displayLine("100 Display", 1);  


  // digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(200);
  blink(1);
  displayLine("200 LED Builtin", 2);

  // Relais PIN for output and off
  pinMode(RELAIS_PIN, OUTPUT);
  digitalWrite(RELAIS_PIN, HIGH);
  displayLine("300 Relais aus", 3);

  delay(750);
  display.clear();

  // do the WIFI startup sequence
  setupWifi();
  delay(200);
  displayLine("400 WLAN", 1);

  // do the MQTT server startup sequence
  setupMQTT();
  delay(200);
  displayLine("500 MQTT", 2);

  // get the Button prepared
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISR_pushed, HIGH);
  displayLine("600 Button", 3);

  delay(750);
  display.clear();

  pinMode(DHTPIN_Inside, INPUT);
  inside.begin();
  delay(200);
  displayLine("700 Inside", 1);

  pinMode(DHTPIN_Outside, INPUT);
  outside.begin();
  delay(200);
  displayLine("800 Outside", 2);

  displayLine("999 Setup done", 3);
  delay(1000);
  blink(1);
}


////////////////////////////////////////////////////////////////////////////////
void loop()
{
  timeBlink(2,100);
  display.clear();
  display.display();

  Drinnen = (readSensor(inside));
  if (isnan(Drinnen.Temperature))
    {
      Serial.println("Fehler Drinnen");
    }
  displayLine(String(Drinnen.Temperature + (uint8_t)0), 1);

  Draussen = (readSensor(outside));
  if (isnan(Draussen.Temperature))
    {
      Serial.println("Fehler Draussen");
    }
  displayLine(String(Draussen.Temperature + (uint8_t)0), 2);
  
  //*************************************************************************************
  // MQTT sending message
  if (!client.connected())
  {
    Serial.println("Reconnect");
    reconnect();
  }

  client.loop();
  //Drinnen  
  sprintf(cstr, "%f", Drinnen.Temperature); //Transfer the double to char for publishing
  client.publish(Temperatur_i_topic, cstr); //Published Temperature
  Serial.println(cstr);
  
  sprintf(cstr, "%f", Drinnen.Humidity); //Transfer the double to char for publishing
  client.publish(Humidity_i_topic, cstr); //Published Preassure

  //Draussen  
  sprintf(cstr, "%f", Draussen.Temperature); //Transfer to char for publishing
  client.publish(Temperatur_a_topic, cstr); //Published Temperature
  Serial.println(cstr);

  sprintf(cstr, "%f", Draussen.Humidity); //Transfer to char for publishing
  client.publish(Humidity_a_topic, cstr); //Published Preassure

//******************************************************  
  //Taupunkt
  difference = tp(Drinnen, Draussen);
  if (isnan(difference))
  {
    delay(2000);
    Serial.println("Sensorfehler");
  } else
  {
  Serial.println(difference + (uint8_t)0);
  displayLine(String(difference), 3);    
  }

  //Was machen wir? Lüften und Pause!
  if (difference > 6 && not isnan(difference))
  {
    Serial.println("Lueften");
    sprintf(cstr, "%f", difference); //Transfer to char
    client.publish(Topic_Luft, cstr); //Publish Lüfter ist dran zu laufen
    displayOut("Auto Lüften");
    digitalWrite(RELAIS_PIN, LOW); //Lüfter an

    Counter = 0;
    while (Counter < 3600) //1 Stunde laufen lassen
    {
      Counter++;
      pause(1000, "1 Stunde Lüften", Counter);
//      displayProgress(Counter, 3600);
    }
    displayOut("Lüftungspause");
    digitalWrite (RELAIS_PIN, HIGH); //Lüfter aus

    Counter = 0;
    while (Counter < 7200) //Pause zwischen zwei Lüftungsläufen
    {
      Counter++;
      pause(1000, "Pause nach dem Lüften - ", Counter);
//      displayProgress(Counter, 7200);
    }
  }
//******************************************************************
// Normale Pause zwischen zwei Messungen
  Counter = 0;
  while (Counter < Sensorpause && not isnan(difference))
  {
    Counter++;
    pause(1000, "Sensorpause - ", Counter);
    buttonCheck();
//    displayProgress(Counter, 58);
  }
}


//************************************************************************
void pause(int iTime, const char* iText, int iCycle)
{
      delay(iTime);
      Serial.print(iText);
      Serial.println(iCycle);
}

void buttonCheck()
{
// manuelles Lüften über Knopf drücken
  if (Button)
  {
    Serial.println("Knopf gedrückt");
    Button = !Button; //switch on<>off
    sprintf(cstr, "%f", "Button"); //Transfer to char
    client.publish(Topic_Luft, cstr); //Publish manuelles lüften
    displayOut("Manuelles Lüften");
    digitalWrite(RELAIS_PIN, LOW); //Lüfter an 
    int ButtonCounter = 0;
    while (ButtonCounter < 3600) //1 Stunde laufen lassen
    {
      ButtonCounter++;
      pause(1000, "Pushlüften", ButtonCounter);
    }
    digitalWrite(RELAIS_PIN, HIGH); //Lüfter an
  }

}