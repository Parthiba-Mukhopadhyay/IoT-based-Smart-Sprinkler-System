#include <SPI.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include "DHT.h"
#define DHTTYPE DHT11
WiFiClient client;
char ssid[] = "Mainak";  // your network SSID (name)
char pass[] = "09072003";     // your network password
long myChannelNumber = 2457132;
const char myWriteAPIKey[] = "NEDILGKQU8UL4CPC";

uint8_t DHTPin =33;
uint8_t soilMoisturePin =34;
uint8_t LDRPin =32;
uint8_t LEDPin =5;
DHT dht(DHTPin, DHTTYPE);
float Temperature;
float Humidity;
float moisture;
float lightIntensity;
void setup() {
Serial.begin(115200);
pinMode(DHTPin, INPUT);
pinMode(soilMoisturePin, INPUT);
pinMode(LDRPin, INPUT);
pinMode(LEDPin, OUTPUT);
delay(100);
WiFi.begin(ssid, pass);
while(WiFi.status() != WL_CONNECTED)
{
delay(200);
Serial.print("..");
}
dht.begin();
ThingSpeak.begin(client);
}
 
void loop() {
Temperature = dht.readTemperature();
Humidity = dht.readHumidity();
moisture = analogRead(soilMoisturePin);
lightIntensity=analogRead(LDRPin);
Serial.print("lightIntensity: ");
Serial.println(lightIntensity);
  if (lightIntensity < 40) {
    digitalWrite(LEDPin , HIGH);
    Serial.println(" => Dark");
  } else if (lightIntensity < 80) {
    digitalWrite(LEDPin , HIGH);
    Serial.println(" => Dim");
  } else if (lightIntensity < 100) {
    digitalWrite(LEDPin , LOW);
    Serial.println(" => Light");
  } else if (lightIntensity < 150) {
    digitalWrite(LEDPin , LOW);
    Serial.println(" => Bright");
  } else {
    digitalWrite(LEDPin , LOW);
    Serial.println(" => Very bright");
  }
ThingSpeak.writeField(myChannelNumber, 5, lightIntensity, myWriteAPIKey);
Serial.print("Temperature: ");
Serial.print(Temperature);
ThingSpeak.writeField(myChannelNumber, 1, Temperature, myWriteAPIKey);
Serial.println("°C");
Serial.print("Humidity: ");
Serial.print(Humidity);
ThingSpeak.writeField(myChannelNumber, 2, Humidity, myWriteAPIKey);
Serial.println("%");
Serial.print("Soil Moisture: ");
Serial.println(moisture);
ThingSpeak.writeField(myChannelNumber, 4, moisture, myWriteAPIKey);
}