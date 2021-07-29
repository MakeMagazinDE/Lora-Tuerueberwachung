//-----------------------------------------------------------------------------
//  Dusk2DawnLoc.cpp
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
Dusk2Dawn destination(LATITUDE, LONGITUDE, TIMEZONE);

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation
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