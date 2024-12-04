#include "WiFi.h"
#include <OneWire.h>
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

byte thingSpeakIdx = 0;
char thingspeakBuffer[128];
const char* ssid = "BugsBunny";
const char* password = "izMir1997";
const uint port = 80;
const char* ip = "52.20.4.242";
WiFiClient localClient;

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
    led_set_pattern(LED_PATTERN_ERROR);

    wifilastTime = millis();

    int tryCount = 0;

    if(WiFi.status() != WL_CONNECTED)
    {
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password); 
        delay(5000);

        tryCount += 1;

        if(tryCount >= 5)
        {
          break;
        }
      } 
      Serial.println("\nConnected.");
    }

    if(WiFi.status() == WL_CONNECTED)
    {
      if (localClient.connect(ip, port))
      {
        Serial.println("localClient connected");
        //localClient.write('A');

        thingSpeakIdx = 0;
        memcpy(&thingspeakBuffer[thingSpeakIdx], "GET ", 4);
        thingSpeakIdx += 4;

        memcpy(&thingspeakBuffer[thingSpeakIdx], "https://api.thingspeak.com/update?", 34);
        thingSpeakIdx += 34;

        memcpy(&thingspeakBuffer[thingSpeakIdx], "api_key=", 8);
        thingSpeakIdx += 8;

        memcpy(&thingspeakBuffer[thingSpeakIdx], "UL2SZD2KC9JEWJDP", 16);
        thingSpeakIdx += 16;

        thingSpeakIdx +=sprintf(&thingspeakBuffer[thingSpeakIdx], "&field1=%.2f&field2=%.2f\r\n", temperature, lux);

        localClient.write_P(thingspeakBuffer, thingSpeakIdx);
        Serial.println("msg sent");
        delay(1000);
        localClient.readString();
        delay(1000);
        led_set_pattern(LED_PATTERN_NORMAL);
      }      
      localClient.stop();
    }
  }
}
