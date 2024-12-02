#include "WiFi.h"
#include <OneWire.h>
#include "ThingSpeak.h"
#include <DallasTemperature.h>

#define LED                   2
#define LED_PATTERN_ERROR     0x55555555
#define LED_PATTERN_NORMAL    0x00000001

unsigned char ledIdx = 0;
unsigned long ledPattern = 0x00000001;
unsigned long ledlastTime = 0;
unsigned long wifilastTime = 0;
unsigned long sensorlastTime = 0;
float lux = 1.0;
float temperature = 0.0;

WiFiClient client;
unsigned long myChannelNumber = 2649032;
const char * myWriteAPIKey = "UL2SZD2KC9JEWJDP";
const char* ssid = "BugsBunny";
const char* password = "izMir1997";

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup()
{
  delay(1000);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  led_set_pattern(LED_PATTERN_ERROR);
  sensors.begin();
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop()
{
  led_handler();
  sensor_handler();
  wifi_handler();
}

void led_set_pattern(unsigned long ledPatt)
{
  ledPattern = ledPatt;
  ledIdx = 0;
}

void led_handler()
{
  if ((millis() - ledlastTime) > 100)
  {
    if(((ledPattern << ledIdx) & 0x80000000) == 0x80000000)
    {
      digitalWrite(LED, LOW);
    }
    else
    {
      digitalWrite(LED, HIGH);
    }

    ledIdx += 1;

    if(ledIdx >= 32)
    {
      ledIdx = 0;
    }

    ledlastTime = millis();
  }
}

void sensor_handler()
{
  if ((millis() - sensorlastTime) > 10000)
  {
    // Get a new temperature reading
    Serial.print("Temperature (ºC): ");
    sensors.requestTemperatures(); 
    temperature = sensors.getTempCByIndex(0);    
    Serial.println(temperature);

    sensorlastTime = millis();
  }
}

void wifi_handler()
{
  if ((millis() - wifilastTime) > (15*60*1000))
  {
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

    int x = 0;
    int retry = 0;

    while(x != 200)
    {
      x = ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);
      delay(1000);

      if(x == 200)
      {
        Serial.println("Field 1 update successful.");
        led_set_pattern(LED_PATTERN_NORMAL);
      }
      else
      {
        Serial.println("Problem Field 1 updating channel. HTTP error code " + String(x));
        led_set_pattern(LED_PATTERN_ERROR);
      }

      retry += 1;
      if(retry >= 5)
      {
        break;
      }     
    }

    delay(1000);
    
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

    lux = temperature - 5.2;

    x = 0;
    retry = 0;

    while(x != 200)
    {
      x = ThingSpeak.writeField(myChannelNumber, 2, lux, myWriteAPIKey);
      delay(1000);

      if(x == 200)
      {
        Serial.println("Field 2 update successful.");
        led_set_pattern(LED_PATTERN_NORMAL);
      }
      else
      {
        Serial.println("Problem Field 2 updating channel. HTTP error code " + String(x));
        led_set_pattern(LED_PATTERN_ERROR);
      }

      retry += 1;
      if(retry >= 5)
      {
        break;
      }     
    }

    wifilastTime = millis();
  }
}
