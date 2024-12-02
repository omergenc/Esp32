#include "WiFi.h"
#include <OneWire.h>
#include "ThingSpeak.h"
#include <DallasTemperature.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define LED         2
#define BUTTON      0
#define ANALOGPIN   0
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const char* ssid = "BugsBunny";
const char* password = "izMir1997";
String hostname = "ESP32 Omer Genc";
const uint port = 7001;
const char* ip = "192.168.1.10";

WiFiClient client;
unsigned long myChannelNumber = 2649032;
const char * myWriteAPIKey = "UL2SZD2KC9JEWJDP";
int adcValue = 0;
int ledDelay = 100;
unsigned long lastTime = 0;
unsigned long timerDelay = (15*60*1000);

float temperatureC = 32.5;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     

// Setup a oneWire instance to communicate with any OneWire devices
//OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
//DallasTemperature sensors(&oneWire);

void setup()
{
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);   //Initialize serial
  pinMode(LED, OUTPUT);/*
  sensors.begin();        // Start the DS18B20 sensor
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname.c_str());  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
*/
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop()
{
    // put your main code here, to run repeatedly:
  digitalWrite(LED, HIGH);
  delay(ledDelay);
  digitalWrite(LED, LOW);
  delay(ledDelay);
/*
  if ((millis() - lastTime) > timerDelay)
  {
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED)
    {
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

    // Get a new temperature reading
    sensors.requestTemperatures(); 
    temperatureC = sensors.getTempCByIndex(0);

    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);
    
    int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureC, myWriteAPIKey);

    if(x == 200)
    {
      Serial.println("Channel update successful.");
      ledDelay = 1000;
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      ledDelay = 100;
    }

    lastTime = millis();
  }*/
}