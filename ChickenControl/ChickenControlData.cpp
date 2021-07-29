//-----------------------------------------------------------------------------
//  ChickenControlData.cpp
/// @file
///
/// @brief Access function for the chicken control data
///
/// @status  Draft
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include "arduino.h"

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "ChickenControlData.h"
#include "ChickenControlConfig.h"

//-----------------------------------------------------------------------------
// Private definitions, macros and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------
/// Semaphore to protect the chicken house data
SemaphoreHandle_t semaChickenHouseData;
bool hasDeviceConnect = false;
struct tm timestampLastChickenHouseContact;
uint8_t chickenHouseDoorState;
uint8_t batteryFuelPct;
float batteryVoltageV;
uint16_t batteryVoltageMv = 0;
uint8_t temperatureDegreeCelcius;
int16_t lastRssi;

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function GetChickenHouseBatteryFuelPct
///
/// @brief Gets the chicken house battery fuel in percent
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
uint8_t GetChickenHouseBatteryFuelPct(void)
{
   uint8_t retVal;

   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
   retVal = batteryFuelPct;

   xSemaphoreGive(semaChickenHouseData);  

   return retVal;
}

//-----------------------------------------------------------------------------
//  Function SetChickenHouseData
///
/// @brief Sets the chicken house data
///
/// @param[in] temperature - Temperature value (unit °C)
///
/// @param[in] batteryVoltage - Battery voltage value (unit mV)
///
/// @param[in] rssi - Rssi value
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void SetChickenHouseData(int8_t temperature, uint16_t batteryVoltage, int16_t rssi)
{
   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);

   batteryVoltageMv = batteryVoltage;
   temperatureDegreeCelcius = temperature;

   lastRssi = (int)rssi;

   batteryVoltageV = (float)batteryVoltageMv / 1000;

   if (batteryVoltageV > BATTERY_VOLTAGE_100_PCT)
   {
      batteryFuelPct = 100;
   }
   else if (batteryVoltageV < BATTERY_VOLTAGE_0_PCT)
   {
      batteryFuelPct = 0;
   }
   else
   {
      batteryFuelPct = (uint8_t)(((batteryVoltageV - BATTERY_VOLTAGE_0_PCT) * 100) / (BATTERY_VOLTAGE_100_PCT - BATTERY_VOLTAGE_0_PCT));
   }

   xSemaphoreGive(semaChickenHouseData);
}

//-----------------------------------------------------------------------------
//  Function GetChickenHouseConnected
///
/// @brief Gets the flag 'Chicken house is connected'
///
/// @return Connection state: true - Chicken house is connected, false - Chicken
/// house isn't connected 
///
/// @see GetChickenHouseConnected()
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
bool GetChickenHouseConnected(void)
{
   bool retVal;

   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
   retVal = hasDeviceConnect;

   xSemaphoreGive(semaChickenHouseData);  

   return retVal;
}

//-----------------------------------------------------------------------------
//  Function SetChickenHouseConnected
///
/// @brief Sets the flag 'Chicken house is connected'
///
/// @param[in] chickenHouseConnected - true - Chicken house is connected,
///                                    false - Chicken house isn't connected
///
/// @see GetChickenHouseConnected()
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void SetChickenHouseConnected(bool chickenHouseConnected)
{
   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);

   hasDeviceConnect = chickenHouseConnected;

   xSemaphoreGive(semaChickenHouseData);  
}

//-----------------------------------------------------------------------------
//  Function GetTimestampLastChickenHouseContact
///
/// @brief Gets the timestamp of the last chicken house contact
///
/// @param[out] timestamp - Timestamp of the last chicken house contact
///
/// @see SetChickenHouseDoorState()
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void GetTimestampLastChickenHouseContact(struct tm * timestamp)
{
    xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
    *timestamp = timestampLastChickenHouseContact;

    xSemaphoreGive(semaChickenHouseData);  
}

//-----------------------------------------------------------------------------
//  Function SetChickenHouseDoorState
///
/// @brief Sets the door state of the chicken house
///
/// @param[in] doorState - Door state: 0 - Door is open, 1 - Door is close
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void SetChickenHouseDoorState(uint8_t doorState)
{
   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);

   chickenHouseDoorState = doorState;

   if(!getLocalTime(&timestampLastChickenHouseContact))
   {
      Serial.println("Failed to obtain time");
      return;
   }

   xSemaphoreGive(semaChickenHouseData);
}

//-----------------------------------------------------------------------------
//  Function GetChickenHouseDoorState
///
/// @brief Gets the door state of the chicken house
///
/// @return Door state: 0 - Door is open, 1 - Door is close
///
/// @see SetChickenHouseDoorState()
///
/// @date    2021-01-15
/// @author  Eike Mueller(TABO)
/// @status  Draft
///
//-----------------------------------------------------------------------------
uint8_t GetChickenHouseDoorState(void)
{
   uint8_t retVal;

   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
   retVal = chickenHouseDoorState;

   xSemaphoreGive(semaChickenHouseData);  

   return retVal;
}

//-----------------------------------------------------------------------------
//  Function GetLastRssi
///
/// @brief Gets the last rssi value
///
/// @return Last rssi value
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
int16_t GetLastRssi(void)
{
   float retVal;

   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
   retVal = lastRssi;

   xSemaphoreGive(semaChickenHouseData);  

   return retVal;
}

//-----------------------------------------------------------------------------
//  Function GetChickenHouseBatteryVoltage
///
/// @brief Gets the battery voltage value (unit V)
///
/// @return Battery voltage value (unit V)
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
float GetChickenHouseBatteryVoltage(void)
{
   float retVal;

   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
   retVal = batteryVoltageV;

   xSemaphoreGive(semaChickenHouseData);  

   return retVal;
}

//-----------------------------------------------------------------------------
//  Function GetChickenHouseTemperature
///
/// @brief Gets the chicken house temperature (unit °C)
///
/// @return Chicken house temperature (unit °C)
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
int8_t GetChickenHouseTemperature(void)
{
   uint8_t retVal;

   xSemaphoreTake(semaChickenHouseData, portMAX_DELAY);
 
   retVal = temperatureDegreeCelcius;

   xSemaphoreGive(semaChickenHouseData);  

   return retVal;
}