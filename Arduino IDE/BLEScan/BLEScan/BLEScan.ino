#include <Arduino.h>
#include <sstream>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define LED         2
#define SCAN_TIME   30     // seconds

int scanTime = 20;  //In seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.println(advertisedDevice.toString().c_str());
  }
};

void setup()
{
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);   //Initialize serial
  pinMode(LED, OUTPUT);

  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);

  // put your main code here, to run repeatedly:
  BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  int count = foundDevices->getCount();
  Serial.println(count);
  Serial.println("Scan done!");

  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice d = foundDevices->getDevice(i);
    Serial.print(d.getAddress().toString());
    Serial.print(" , ");
    Serial.print(d.getRSSI());
    Serial.print(" , ");
    Serial.println(d.getName());

/*
    if (d.haveManufacturerData())
    {
      std::string md = d.getManufacturerData();
      uint8_t *mdp = (uint8_t *)d.getManufacturerData().data();
      char *pHex = BLEUtils::buildHexData(nullptr, mdp, md.length());
      ss << ",\"ManufacturerData\":\"" << pHex << "\"";
      free(pHex);
    }*/
/*
    if (d.haveServiceUUID())
    {
      ss << ",\"ServiceUUID\":\"" << d.getServiceUUID().toString() << "\"";
    }

    if (d.haveTXPower())
    {
      ss << ",\"TxPower\":" << (int)d.getTXPower();
    }
*/
  }

  pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
}