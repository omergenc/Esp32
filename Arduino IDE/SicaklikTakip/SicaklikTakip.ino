#include "WiFi.h"
#include <Wire.h>
#include <BH1750.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LED                   2
#define LED_PATTERN_ERROR     0x00110011
#define LED_PATTERN_NORMAL    0x00010001
#define RELAY                 16
#define BUTTON                23
#define SEG_COM_1             0
#define SEG_COM_2             15
#define SEG_COM_3             19
#define SEG_COM_4             18
#define SEGMENT_A             13
#define SEGMENT_B             32
#define SEGMENT_C             26
#define SEGMENT_D             14
#define SEGMENT_E             12
#define SEGMENT_F             33
#define SEGMENT_G             25
#define SEGMENT_DP            27


unsigned char segments[4];
unsigned char segComIdx = 0;
unsigned char displayIdx = 0;
unsigned char ledIdx = 0;
unsigned long ledPattern = 0x00000001;
unsigned long ledlastTime = 0;
unsigned long displaylastTime = 0;
unsigned long refreshlastTime = 0;
unsigned long wifilastTime = 0;
unsigned long sensorlastTime = 0;
float lux = 1.0;
float temperature = 0.0;

byte thingSpeakIdx = 0;
char thingspeakBuffer[128];
const char* ssid = "BugsBunny";
const char* password = "izMir1997";
const uint port = 80;
//const char* ip = "52.20.4.242";
const char* ip = "52.2.152.117";
//const char* ip = "54.235.105.242";
WiFiClient localClient;

BH1750 lightMeter;

const int oneWireBus = 4;             // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus);          // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature sensor 

                                            //  0,    1,    2,    3,    4,    5,    6,    7     8,    9,    A,    B,    C,    D,    E,    F,   10
const uint8_t sevenSegmentLookUpTable[17] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, 0x00};

void setup()
{
  delay(1000);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(SEG_COM_1, OUTPUT);
  pinMode(SEG_COM_2, OUTPUT);
  pinMode(SEG_COM_3, OUTPUT);
  pinMode(SEG_COM_4, OUTPUT);
  pinMode(SEGMENT_A, OUTPUT);
  pinMode(SEGMENT_B, OUTPUT);
  pinMode(SEGMENT_C, OUTPUT);
  pinMode(SEGMENT_D, OUTPUT);
  pinMode(SEGMENT_E, OUTPUT);
  pinMode(SEGMENT_F, OUTPUT);
  pinMode(SEGMENT_G, OUTPUT);
  pinMode(SEGMENT_DP, OUTPUT);
  led_set_pattern(LED_PATTERN_ERROR);
  sensors.begin();
  Wire.begin();
  lightMeter.begin();
  WiFi.mode(WIFI_STA);
}

void loop()
{
  led_handler();
  display_handler();
  sensor_handler(false);
  wifi_handler(false);

  if (digitalRead(BUTTON) == LOW)
  {
    sensor_handler(true);
    wifi_handler(true);
  }
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

void sensor_handler(bool force)
{
  if (((millis() - sensorlastTime) > 10000) || (force == true))
  {
    sensors.requestTemperatures(); 
    lux = lightMeter.readLightLevel();
    temperature = sensors.getTempCByIndex(0);    

    Serial.print("Light : ");
    Serial.print(lux);
    Serial.print(" - Temperature (ºC): ");
    Serial.println(temperature);

    sensorlastTime = millis();
  }
}

void wifi_handler(bool force)
{
  if (((millis() - wifilastTime) > (15*60*1000)) || (force == true))
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

void segment_display(unsigned char bitMask)
{
  if((bitMask & 0x01) == 0x01)
  {
    digitalWrite(SEGMENT_A, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_A, LOW);
  }

  if((bitMask & 0x02) == 0x02)
  {
    digitalWrite(SEGMENT_B, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_B, LOW);
  }

  if((bitMask & 0x04) == 0x04)
  {
    digitalWrite(SEGMENT_C, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_C, LOW);
  }

  if((bitMask & 0x08) == 0x08)
  {
    digitalWrite(SEGMENT_D, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_D, LOW);
  }

  if((bitMask & 0x10) == 0x10)
  {
    digitalWrite(SEGMENT_E, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_E, LOW);
  }

  if((bitMask & 0x20) == 0x20)
  {
    digitalWrite(SEGMENT_F, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_F, LOW);
  }

  if((bitMask & 0x40) == 0x40)
  {
    digitalWrite(SEGMENT_G, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_G, LOW);
  }

  if((bitMask & 0x80) == 0x80)
  {
    digitalWrite(SEGMENT_DP, HIGH);
  }
  else
  {
    digitalWrite(SEGMENT_DP, LOW);
  }
}

void display_segment_handler(void)
{
  if ((millis() - refreshlastTime) > 4)
  {
    refreshlastTime = millis();

    digitalWrite(SEG_COM_1, LOW);
    digitalWrite(SEG_COM_2, LOW);
    digitalWrite(SEG_COM_3, LOW);
    digitalWrite(SEG_COM_4, LOW);

    switch(segComIdx)
    {
      case 0:
        digitalWrite(SEG_COM_1, HIGH);
        segment_display(segments[segComIdx]);
      break;

      case 1:
        digitalWrite(SEG_COM_2, HIGH);
        segment_display(segments[segComIdx]);
      break;

      case 2:
        digitalWrite(SEG_COM_3, HIGH);
        segment_display(segments[segComIdx]);
      break;

      case 3:
        digitalWrite(SEG_COM_4, HIGH);
        segment_display(segments[segComIdx]);
      break;
    }

    segComIdx += 1;
    if(segComIdx >= 4)
    {
      segComIdx = 0;
    }
  }
}

void display_write_value(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t dp_Mask)
{
  uint8_t dp_d1 = 0;
  uint8_t dp_d2 = 0;
  uint8_t dp_d3 = 0;
  uint8_t dp_d4 = 0;

  if((dp_Mask & 0x08) == 0x08)
  {
    dp_d1 = 0x80;
  }

  if((dp_Mask & 0x04) == 0x04)
  {
    dp_d2 = 0x80;
  }

  if((dp_Mask & 0x02) == 0x02)
  {
    dp_d3 = 0x80;
  }

  if((dp_Mask & 0x01) == 0x01)
  {
    dp_d4 = 0x80;
  }

  segments[0] = sevenSegmentLookUpTable[d1] | dp_d1;
  segments[1] = sevenSegmentLookUpTable[d2] | dp_d2;
  segments[2] = sevenSegmentLookUpTable[d3] | dp_d3;
  segments[3] = sevenSegmentLookUpTable[d4] | dp_d4;
}

void display_handler(void)
{
  float tempTemperature = temperature;
  char tempBuffer[16];

  if ((millis() - displaylastTime) > 2000)
  {
    displaylastTime = millis();

    switch(displayIdx)
    {
      case 0:
        if(temperature < 0.0)
        {
          tempTemperature = 0;
        }

        if(tempTemperature > 10.0)
        {
          sprintf(tempBuffer, "%.2f", tempTemperature);
        }
        else
        {
          sprintf(tempBuffer, "0%.2f", tempTemperature);
        }

        tempBuffer[0] = tempBuffer[0] - '0';
        tempBuffer[1] = tempBuffer[1] - '0';
        tempBuffer[3] = tempBuffer[3] - '0';
        tempBuffer[4] = tempBuffer[4] - '0';

        display_write_value(tempBuffer[0], tempBuffer[1], tempBuffer[3], tempBuffer[4], 0x04);
      break;
    }
/*
    displayIdx += 1;
    if(displayIdx >= 2)
    {
      displayIdx = 0;
    }*/
  }

  display_segment_handler();
}
