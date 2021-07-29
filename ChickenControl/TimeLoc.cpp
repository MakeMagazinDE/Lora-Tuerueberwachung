//-----------------------------------------------------------------------------
//  TimeLoc.cpp
/// @file
///
/// @brief <Please fill out with the short description!>
///
/// <Please fill out with the extended description!>
///
/// @see <Cross reference to other classes ...>
///
/// @copyright 2017-2021 Hodapp GmbH & Co. KG - All Rights Reserved
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
#include "Arduino.h"
#include <TimeLib.h>

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "TimeLoc.h"
#include "ChickenControlConfig.h"
#include "DayOfWeek.h"
#include "Dusk2DawnLoc.h"

#ifdef USE_NTP_SERVER
 #include <WiFi.h>
#endif

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
#ifdef USE_NTP_SERVER
 // Graevenwiesbach
 const char* ssid       = WLAN_SSID;
 const char* password   = WLAN_PW;

 // Graevenwiesbach
 //const char* ssid       = "FRITZ!Box 7272";
// const char* password   = "44579886255681963683";

// const char* ntpServer = "pool.ntp.org";
 const char* ntpServer = "de.pool.ntp.org";

 const long  gmtOffset_sec = 3600;
 const int   daylightOffset_sec = 3600;
#endif

bool isDst;

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
void CorrectTimeForDst(void);
void printSunSetSunDown(void);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

void GetTimeFromNTP(void)
{
#ifdef USE_NTP_SERVER
   Serial.printf("Connecting to %s ", ssid);
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED)
   {
      delay(500);
      Serial.print(".");
   }
   
   Serial.println(" CONNECTED");
  
   //init and get the time
   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

   CorrectTimeForDst();

   printLocalTime();

   printSunSetSunDown();

   //disconnect WiFi as it's no longer needed
   WiFi.disconnect(true);
   WiFi.mode(WIFI_OFF);
#else
   struct timeval tv;
   tv.tv_sec = 1522255956;
   settimeofday(&tv, NULL);
#endif
}

bool IsDst(void)
{
   return isDst;
}

void CorrectTimeForDst(void)
{
   struct tm timeinfo;
   time_t t;
   int lastSundayInMarch;
   int lastSundayInOctober;
   struct tm timeinfoStartDst;
   struct tm timeinfoEndDst;

   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return;
   }

   timeinfoStartDst.tm_year = timeinfo.tm_year;
   timeinfoStartDst.tm_mon = 2;
   timeinfoStartDst.tm_mday = GetLastWeekDayInMonth(1, 3, timeinfo.tm_year + 1900);
   timeinfoStartDst.tm_hour = 3;
   timeinfoStartDst.tm_min = 0;
   timeinfoStartDst.tm_sec = 0;

   timeinfoEndDst.tm_year = timeinfo.tm_year;
   timeinfoEndDst.tm_mon = 9;
   timeinfoEndDst.tm_mday = GetLastWeekDayInMonth(1, 10, timeinfo.tm_year + 1900);
   timeinfoEndDst.tm_hour = 2;
   timeinfoEndDst.tm_min = 0;
   timeinfoEndDst.tm_sec = 0;

   Serial.println("Start Sommerzeit:");
   Serial.println(&timeinfoStartDst, "%A, %B %d %Y %H:%M:%S");

   Serial.println("Ende Sommerzeit:");
   Serial.println(&timeinfoEndDst, "%A, %B %d %Y %H:%M:%S");

   time_t timeinfoVal = mktime(&timeinfo);
   time_t timeinfoStartDstVal = mktime(&timeinfoStartDst);
   time_t timeinfoEndDstVal = mktime(&timeinfoEndDst);

   if (timeinfoVal > timeinfoStartDstVal && timeinfoVal < timeinfoEndDstVal)
      isDst = true;
   else
      isDst = false;

   if (IsDst() == false)
   {
      Serial.println("Winterzeit!");
   }
   else
   {
      Serial.println("Sommerzeit!");
   }
}

void printLocalTime(void)
{
   struct tm timeinfo;
  
   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return;
   }
  
   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void printSunSetSunDown(void)
{
   int sunrise = GetSunriseForToday();
   int sunset  = GetSunsetForToday();

   Serial.print("Sonnenaufgang: ");
   Serial.print(sunrise / 60, DEC);
   Serial.print(":");
   Serial.print(sunrise % 60, DEC);
   Serial.println();
 
   Serial.print("Sonnenuntergang: ");
   Serial.print(sunset / 60, DEC);
   Serial.print(":");
   Serial.print(sunset % 60, DEC);
   Serial.println();
}