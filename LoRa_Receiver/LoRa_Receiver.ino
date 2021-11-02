/*

  Finn Arp,
  Maturaarbeit, 2021

  ESP32 to Database:
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  LoRa:
  Alexander Egger
  More information at: https://www.aeq-web.com
  Build: 2019-DEC-20

*/

//LoRa
#include <SPI.h>
#include <LoRa.h>

//post
#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>

//LoRa pins
#define ss 18
#define rst 23
#define dio0 26

// network credentials
const char* ssid     = "arpnetneu";
const char* password = "PASSWORD";

// Domain name and URL path or IP address with path
const char* serverName = "https://matura.work/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName;
String sensorLocation;

String RxString;

void setup() {
  //LoRa
  Serial.begin(9600);
  Serial.println("LoRa Rx");
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  //WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) { // If LoRa Packet received
    Serial.println("Receive packet");
    get_lora_data();
    post_lora_data();
  }
}

void get_lora_data() {
  boolean clr = 1;
  while (LoRa.available()) {
    if (clr == 1) {
      RxString = "";
      clr = 0;
    }
    RxString += (char)LoRa.read();
  }
  clr = 1;
}


void post_lora_data() {

  sensorName = "T-Beam";
  String station_id = split_string( RxString, ';', 0);
  String temp = split_string( RxString, ';', 1);
  //String value2 = split_string( RxString, ';', 2);
  //String value3 = split_string( RxString, ';', 3);

  Serial.print("Station-ID: ");
  Serial.println(station_id);
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" *C");

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Domain name with URL path or IP address with path
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                             + "&location=" + sensorLocation + "&value1=" + temp;
    //+ "&value2=" + value2 + "&value3=" + value2 + "";

    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

String split_string(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length();

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
