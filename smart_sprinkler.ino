#include <SPI.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#define BLYNK_TEMPLATE_ID "TMPL3QuiXn9rX"
#define BLYNK_TEMPLATE_NAME "Sprinkler"
#define BLYNK_AUTH_TOKEN "DymB4_aavnPQoAtPr8PgRcGtn7cO7S4J"
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;
#include "DHT.h"
// put the code you got in Step 3 into this file
#include "model.h"
#define DHTTYPE DHT11
WiFiClient client;
char ssid[] = "helomoto";  // your network SSID (name)
char pass[] = "abcd1234";     // your network password
long myChannelNumber = 2457132;
const char myWriteAPIKey[] = "NEDILGKQU8UL4CPC";
char output[256];
uint8_t DHTPin =33;
uint8_t soilMoisturePin =34;
uint8_t LDRPin =32;
uint8_t LEDPin =5;
DHT dht(DHTPin, DHTTYPE);
float Temperature;
float Humidity;
float moisture;
float lightIntensity;
Eloquent::ML::Port::RandomForest classifier;
int overall = 0;
int safe = 0;
int classify(float temp, float humidity, float moisture, float intensity ) {
    float x_sample[] = { temp, humidity, moisture, intensity};
    Serial.print("Predicted class: ");
    Serial.println(classifier.predict(x_sample));
    return (classifier.predict(x_sample));
}

void sendSensor()
{
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  moisture = analogRead(soilMoisturePin);
  lightIntensity=analogRead(LDRPin);
  Serial.print("lightIntensity: ");
  Serial.println(lightIntensity);
  Serial.print("Temperature: ");
  Serial.print(Temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.println("%");
  Serial.print("Soil Moisture: ");
  Serial.println(moisture);
  int led = classify(Temperature, Humidity, moisture, lightIntensity);
  
  int count=0;
   bool flag = false;
  Blynk.virtualWrite(V1, Temperature);
  Blynk.virtualWrite(V0, Humidity);
  delay(1000);
  Blynk.virtualWrite(V2, lightIntensity);
  Blynk.virtualWrite(V3, moisture);
  Blynk.virtualWrite(V4, led);
  digitalWrite(LEDPin,led);
  while(led==1)
  {
    count++;
    Temperature = dht.readTemperature();
    Humidity = dht.readHumidity();
    moisture = analogRead(soilMoisturePin);
    lightIntensity=analogRead(LDRPin);
    led = classify(Temperature, Humidity, moisture, lightIntensity);
    flag = true;
    Blynk.virtualWrite(V1, Temperature);
    Blynk.virtualWrite(V0, Humidity);
    delay(1000);
    Blynk.virtualWrite(V2, lightIntensity);
    Blynk.virtualWrite(V3, moisture);
    delay(1000);
    Blynk.virtualWrite(V4, led);
    digitalWrite(LEDPin, led);
  }
  if(flag == true){
    overall += count*22;  // dynamic from https://newellentoncpw.com/documents/1277/Sprinkler_Usage_Calculations.pdf
    safe += (22*60-count)*22; // https://liquidlawn.com/water-lawn/

   sprintf(output,"Water dispensed (in ml): %d\n overall water used: %d\n Saved water: %d\nOverall saved: %d\n",
            count*22, overall, (22*60-count)*22, safe);
    Serial.println(output);
    delay(200);
    flag=false;
    
    Blynk.logEvent("water_save_report",output);
  }
}


void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
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
  Eloquent::ML::Port::RandomForest classifier;
  dht.begin();
  timer.setInterval(1000L, sendSensor);

}
 
void loop() {
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V4){
  int value = param.asInt();
  Serial.print("button pressed ");
  Serial.println(value);
    digitalWrite(LEDPin, value);
    Blynk.virtualWrite(V4, value);
    delay(10000); //300000
}
