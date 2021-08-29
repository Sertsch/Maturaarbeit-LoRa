/*
  Finn Arp
  Maturaarbeit 2021
  
  Temperature Sensor:
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Based on the Dallas Temperature Library example
  
  LoRa:
  Alexander Egger
  More information at: https://www.aeq-web.com/
  Main Software for LoRa-Weatherstation 2020 (V 1.0|03052020)
*/

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

//Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

String device_id = "TEST";  //device name

#define ss 18         //LoRa SS PIN
#define rst 23        //LoRa RST PIN
#define dio0 26       //LoRa DIO PIN

int sleep_time = 180; //ESP sleep time (seconds)

//Data variables
float temp;
//float value2;
//float value3;


void setup() {
  Serial.begin(9600);
  SPI.begin(5, 19, 27, 18);     //Pins for LoRa SPI
  LoRa.setPins(ss, rst, dio0);  //Pins for LoRa
  LoRa.begin(868E6);            //Start LoRa @ 868 MHz (EU-Band)

  sensors.begin(); //Start up Sensor library

  esp_sleep_enable_timer_wakeup(sleep_time * 1000000);
}

void loop() {
  get_data();
  send_lora_packet();
  esp_deep_sleep_start();
}

void get_data(){
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  
  temp = (sensors.getTempCByIndex(0));
  Serial.println(temp);
}

void send_lora_packet() {
  LoRa.beginPacket();
  LoRa.print(device_id);
  LoRa.print(";");
  LoRa.print(temp);
  LoRa.print(";");
  //LoRa.print(value2);
  //LoRa.print(";");
  //LoRa.print(value3);
  //LoRa.print(";");
  LoRa.endPacket();
  delay(500);
}
