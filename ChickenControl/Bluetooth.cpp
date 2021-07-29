//-----------------------------------------------------------------------------
//  Bluetooth.cpp
/// @file
///
/// @brief <Please fill out with the short description!>
///
/// <Please fill out with the extended description!>
///
/// @see <Cross reference to other classes ...>
///
/// @copyright 2017-2020 Hodapp GmbH & Co. KG - All Rights Reserved
///
/// @status  Draft
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
/// @creator  Eike Mueller(TABO)
///
/// @todo <[optional] There are open issues?>
/// @todo <[optional] It is possible that the description of an open issue in
///                   coding is longer than a complete line. So when we search
///                   the word 'todo' in code, we found only the first line of
///                   the todo.
///
/// @todo <This comment was placed in by a macro. Please fill out all the field
///        in brackets with useful informations.>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#if 0
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#endif
#if 0
#include <arduino.h>

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "Bluetooth.h"
#include "PlaySound.h"

//-----------------------------------------------------------------------------
// Private definitions, macros and constants
//-----------------------------------------------------------------------------
#define OFFSET_STATE_CHICKEN_HOUSE_DOOR 20

#define OFFSET_BATTERY_FUEL_LOW         21

#define OFFSET_BATTERY_FUEL_HIGH        22

#define BATTERY_VOLT_STEP   (3.60/32767)

#define BATTERY_VOLTAGE_100_PCT  3.1f

#define BATTERY_VOLTAGE_0_PCT  2.0f

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------
int scanTime = 10; //In seconds
#endif
#if 0
BLEScan* pBLEScan;

//BLEAddress AddressChickenHouse("88:6b:0f:4e:24:c5");
BLEAddress AddressChickenHouse("88:6b:0f:f4:0f:13");
#endif
#if 0
uint8_t chickenHouseData[25];

uint8_t chickenHouseDoorState;

uint8_t batteryFuelPct;

uint8_t lastRssi;

float batteryVoltageV;



SemaphoreHandle_t semaChickenHouseData;
extern SemaphoreHandle_t semaPlaySound;

struct tm timestampLastChickenHouseContact;
#endif
//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
#if 0
void PrepareBleScan(void);
void StartBleScan(void);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

      BLEAddress addrAdvertisedDevice = advertisedDevice.getAddress();

      if (addrAdvertisedDevice.equals(AddressChickenHouse) == true)
      {
          Serial.printf("ChickenSymbol house is calling with ");
          if (advertisedDevice.haveRSSI())
              Serial.printf("rssi: %i db", advertisedDevice.getRSSI());
          if (advertisedDevice.haveTXPower())
              Serial.printf(", tx-power: %i", advertisedDevice.getTXPower());
          if (advertisedDevice.haveAppearance())
              Serial.printf(", appearance: %i", advertisedDevice.getAppearance());
          if (advertisedDevice.haveManufacturerData())
          {
              Serial.printf(", mfct-data: %s", advertisedDevice.getManufacturerData().c_str());
              Serial.printf(", mfct-data length: %i bytes", advertisedDevice.getManufacturerData().size());
          }
          if (advertisedDevice.haveName())
              Serial.printf(", name: %s", advertisedDevice.getName().c_str());
          if (advertisedDevice.haveServiceUUID())
              Serial.printf(", uuid: %s", advertisedDevice.getServiceUUID().toString().c_str());
          Serial.printf("\n");
          Serial.printf("Advertising data: %s\n", advertisedDevice.toString().c_str());

          uint8_t * pData = (uint8_t *) advertisedDevice.getManufacturerData().c_str();

          Serial.printf("Data: 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n", pData[20], pData[21], pData[22], pData[23], pData[24]);

          SetChickenHouseDoorState(pData[OFFSET_STATE_CHICKEN_HOUSE_DOOR]);

          //SetChickenHouseData(pData, advertisedDevice.getRSSI());
      }
    }
};

void PrepareBleScan(void)
{
  lastRssi = 0;
  batteryFuelPct = 0;
  batteryVoltageV = 0.0f;

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void StartBleScan(void)
{
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    //Serial.print("Devices found: ");
    //Serial.println(foundDevices.getCount());
    //Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}

void TaskBleScan( void *pvParameters )
{
      // Preparing for BLE scan
  PrepareBleScan();

    for (;;)
    {
        vTaskDelay(10);

        StartBleScan();
    }
} 
#endif
