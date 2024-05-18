#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "env.h"



OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temp()
{
  sensors.requestTemperatures();
  float x = sensors.getTempCByIndex(0);
  return x;
}
bool presence(){
  int person_in_room;
  sensorValue = digitalRead(sensorPin);

  if (sensorValue == HIGH) {
    delay(100);
    
    if (sensorState == false) {
      sensorState = true;

    }
  } 
  else {
      delay(200);
      
      if (sensorState == true){
        sensorState = false;
    }
  }

  return sensorState;
}

void postSensor(float temp,bool presence){
 HTTPClient http;
  String requestBody;

  String new_endpoint;
  String path = "/sensorData";
  new_endpoint = endpoint + path,

  http.begin(new_endpoint);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", "62");

  JsonDocument doc;

  doc["temperature"] = temp;
  doc["presence"] = presence;

  doc.shrinkToFit();

  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);
  
  Serial.print("RESPONSE: ");
  Serial.println(http.getString());
  Serial.println();

  http.end();
}
void whatfan(){
 
  String new_endpoint;
  String path = "/fan";
  new_endpoint = endpoint + path,

  http.begin(new_endpoint);

  int httpResponseCode = http.GET();

  if(httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String responseBody = http.getString();
    Serial.println(responseBody);

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, responseBody);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    const bool fan_ctrl = doc["fan"]; 

    digitalWrite(fan_pin, fan_ctrl); 
    
    }
    else{
      Serial.print("Error Code: ");
      Serial.println(httpResponseCode);
    }
  
    http.end(); http.end();
}
void getLight(){
  HTTPClient http;

  String new_endpoint;
  String path = "/light";
  new_endpoint = endpoint + path,


  http.begin(new_endpoint);

  int httpResponseCode = http.GET();

  if(httpResponseCode > 0) {

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String responseBody = http.getString();
    Serial.println(responseBody);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, responseBody);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    const bool light_ctrl = doc["light"]; 
    digitalWrite(light_pin, light_ctrl);

    }
    else{
      Serial.print("Error Code: ");
      Serial.println(httpResponseCode);
    }
  
    http.end();

}


int readprox()
{ 
   bool person_in_room;
  sensorValue = digitalRead(sensorPin);
  if (sensorValue == HIGH) {
    digitalWrite(2, HIGH);
    delay(100);
    
    if (sensorState == LOW) {
      Serial.println("Motion detected!");
      person_in_room=true;
      sensorState = HIGH;

    }
  } 
  else {
      digitalWrite(2, LOW);
      delay(200);
      
      if (sensorState == HIGH){
        Serial.println("Motion stopped!");
        person_in_room=false;
        sensorState = LOW;
    }
  }
  return person_in_room;
  }



void setup() {
   pinMode(2, OUTPUT);
   pinMode(light_pin, OUTPUT);
  pinMode(fan_pin, OUTPUT);

  pinMode(sensorPin, INPUT);
  pinMode(temp_sensor, INPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //Check WiFi connection status

  

  if(WiFi.status()== WL_CONNECTED){
     
     bool prox= readprox();
     float temperature = temp();
     postSensor(temperature,prox);
    sensors.requestTemperatures();
    whatfan();
    float s = sensors.getTempCByIndex(0);
    
      Serial.println(s);
  
    getLight();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
 
  // put your main code here, to run repeatedly:
}
