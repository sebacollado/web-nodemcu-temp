
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const bool ssl = true;

// Device key
const String dev_key = "423F4528482B4B6250655368566D597133743677397A24432646294A404E6351";

// WiFi configuration
const char* ssid = "CENSORED";
const char* password = "CENSORED";

// Temperature
float temp = 0;

void httpSend(float temp){
  // HTTPClient
  HTTPClient http;
  
  String postData = "temp=" + String(temp, 2) + "&key=" + dev_key;

  http.begin("http://192.168.1.10/temperature");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData);
  String payload = http.getString();

  Serial.println(httpCode);
  Serial.println(payload);

  http.end();
  return;
}

void httpsSend(float temp){
  WiFiClientSecure client;
  client.allowSelfSignedCerts();

  String postData = "{";
    postData += "\"temp\": \""+String(temp, 2)+"\",";
    postData += "\"key\": \""+dev_key+"\"}";
  
  if(!client.connect("192.168.1.10", 443)){
    Serial.println("Connection failed");
  }else{
    Serial.println("Connected to server!");
    client.print(String("POST ") + "/temperature" + " HTTP/1.1\r\n" +
                 "Host: " + "192.168.1.10" + "\r\n" +
                 "Content-Type: application/json; charset=utf-8\r\n" +
                 "Content-Length: " + postData.length() + "\r\n" +
                 "\r\n" +
                 postData + "\n");
  }
  return;
}

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("");

  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected. IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void loop()
{
  // Connection OK
  if (WiFi.status() == WL_CONNECTED) {
    temp = (analogRead(A0) * 330.0) / 1024.0; //lm35
    
    if(ssl) httpsSend(temp); else httpSend(temp);
  }
  // Connection error
  else{
    Serial.println("connection to server failed");
  }
  delay(1000);
}
