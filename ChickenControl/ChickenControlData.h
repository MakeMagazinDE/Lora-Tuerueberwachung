//-----------------------------------------------------------------------------
//  ChickenControlData.h
/// @file
///
/// @brief Header for the file ChickenControlData.cpp
///
/// @status  Draft
/// $Date:$
/// $Revision$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------
#ifndef __CHICKENCONTROLDATA_H
 #define __CHICKENCONTROLDATA_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,macros and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------
// Gets the chicken house battery fuel in percent
uint8_t GetChickenHouseBatteryFuelPct(void);
// Sets the chicken house data
void SetChickenHouseData(int8_t temperature, uint16_t batteryVoltage, int16_t rssi);
// Gets the door state of the chicken house
uint8_t GetChickenHouseDoorState(void);
// Sets the door state of the chicken house
void SetChickenHouseDoorState(uint8_t doorState);
// Gets the timestamp of the last chicken house contact
void GetTimestampLastChickenHouseContact(struct tm * timestamp);
// Gets the flag 'Chicken house is connected'
bool GetChickenHouseConnected(void);
// Sets the flag 'Chicken house is connected'
void SetChickenHouseConnected(bool chickenHouseConnected);
// Gets the last rssi value
int16_t GetLastRssi(void);
// Gets the battery voltage value (unit V)
float GetChickenHouseBatteryVoltage(void);
// Gets the chicken house temperature (unit °C)
int8_t GetChickenHouseTemperature(void);

#endif //__CHICKENCONTROLDATA_H
