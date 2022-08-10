//-----------------------------------------------------------------------------
//  Dusk2DawnLoc.cpp
/// @file
///
/// @brief Contains functions to get information about todays sunset and sunrise
///
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "Dusk2Dawn.h"
#include "Dusk2DawnLoc.h"
#include "TimeLoc.h"

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
/// Dusk2Dawn instance
Dusk2Dawn destination(LATITUDE, LONGITUDE, TIMEZONE);

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function GetSunriseForToday
///
/// @brief Gets the sunrise for today
///
/// @return Todays sunrise in (unit minutes)
///
/// @see GetSunsetForToday()
///
//-----------------------------------------------------------------------------
int GetSunriseForToday(void)
{
   struct tm timeinfo;

   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return 0;
   }

   int sunrise  = destination.sunrise(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, false);

   if (IsDst() == false)
   {
      sunrise -= 60;
   }

   return sunrise;
}

//-----------------------------------------------------------------------------
//  Function GetSunsetForToday
///
/// @brief Gets the sunset for today
///
/// @return Todays sunset in (unit minutes)
///
/// @see GetSunriseForToday()
///
//-----------------------------------------------------------------------------
int GetSunsetForToday(void)
{
   struct tm timeinfo;

   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return 0;
   }

   int sunset  = destination.sunset(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, false);

   if (IsDst() == false)
   {
      sunset -= 60;
   }

   return sunset;
}