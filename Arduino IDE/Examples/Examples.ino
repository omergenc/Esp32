#include "WiFi.h"

#define LED         2
#define BUTTON      0
#define ANALOGPIN   0

const char* ssid = "BugsBunny";
const char* password = "izMir1997";
String hostname = "ESP32 Omer Genc";
const uint port = 7001;
const char* ip = "192.168.1.10";

WiFiClient localClient;
int adcValue = 0;

void setup()
{
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  //pinMode(BUTTON, INPUT_PULLUP);

  WiFi.setHostname(hostname.c_str());

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());
}

void loop()
{
    // put your main code here, to run repeatedly:
/*
  Serial.print("\nESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP32 HostName: ");
  Serial.println(WiFi.getHostname());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());*/

  sendRequest();

  adcValue = analogRead(ANALOGPIN);
  Serial.println(adcValue);

  digitalWrite(LED, HIGH);
  delay(250);
  digitalWrite(LED, LOW);
  delay(250);
}

void sendRequest()
{
  if(!localClient.connected())
  {
    if (localClient.connect(ip, port))
    {
      Serial.println("localClient connected");
      localClient.write('A'); //Single char
      Serial.println("msg sent");
    }
  }

  if(localClient.connected())
  {
    Serial.println("Start reading");
    if (localClient.available() > 0)
    {
      char c = localClient.read();
      Serial.print(c);
      localClient.write(c); // Send received char
    }
    Serial.println("Stop reading");
  }
}