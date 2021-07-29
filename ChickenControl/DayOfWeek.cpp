//-----------------------------------------------------------------------------
//  DayOfWeek.cpp
/// @file
///
/// @brief Day of the week calculation
///
/// Source code is copied from url https://www.hackster.io/erikuchama/day-of-the-week-calculator-cde704
///
/// @status  Draft
/// $Date:$
/// $Revision$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------

/*
  Day of the Week calculator

  Accurate as far back as September 14, 1752.  
  In 1752, England was the last country to
  skip days (11 days) in order to sync up 
  calendars as the world finished shifting from 
  the Julian calendar to the Gregorian calendar.
  (Well...technically England was not the last 
  to change.  Russian calendars did not shift to 
  the Gregorian calendar until 1918, and Greece 
  shifted in 1923)
  
  Any date entered prior to this shift may 
  result in incorrect day of the week calculations

  Using formula:
  DoW value = (d + m + y + (y/4) + c)mod7

  where
  d = day
  m = month value (as per table below)
  y = two digit year
  c = century value (as per table below)

  - - - - - - - - - - - - - - - - - - - - 
 | Century |  Month        | DoW | Value |
 | - - - - | - - - - - - - | - - | - - - | 
 |    20   |  Jan/Oct      | Sat |   0   |
 |    19   |  May          | Sun |   1   |
 |    n/a  |  *Feb/Aug     | Mon |   2   |
 |    22   |  Feb/Mar/Nov  | Tue |   3   |
 |    n/a  |  Jun          | Wed |   4   |
 |    21   |  Sep/Dec      | Thu |   5   |
 |    n/a  |  *Jan/Apr/Jul | Fri |   6   |
  - - - - - - - - - - - - - - - - - - - -

 Note:  For Jan and Feb, if the year is a 
 leap year, use the *Jan or *Feb.  Otherwise 
 use regular Jan or Feb. 
 (see Leap Year note below)

 When all values are entered into formula
 (all values are absolute, so whatever you get
 for y/4, drop the decimal), divide by 7.  
 The remainder (0-6) goes back to the chart 
 to determine Day of the week.

 Leap Year Note:
 A leap year is determined true if:
 -1- The year is evenly divisible by 4, but not divisible by 100
 or
 -2- The year is evenly divisible by 400.

 Created by Eric Sitler
 30August2016

*/

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include <math.h>
#include "DayOfWeek.h"

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

int m;          // Month Entry
int d;          // Day Entry
int yy;         // Last 2 digits of the year (ie 2016 would be 16)
int yyyy;       // Year Entry
int c;          // Century (ie 2016 would be 20)
int mTable;     // Month value based on calculation table
int SummedDate; // Add values combined in prep for Mod7 calc
int DoW;        // Day of the week value (0-6)
int leap;       // Leap Year or not
int cTable;     // Century value based on calculation table

int GetNumberOfDaysPerMonth(int month, int year);

int GetLastWeekDayInMonth(int weekDay, int month, int year)
{
   int numOfDays = GetNumberOfDaysPerMonth(month, year);

   while(numOfDays > 0)
   {
      if (GetDayOfWeekForDate(numOfDays, month, year) == weekDay)
         return numOfDays;

      numOfDays--;
   }
}

int GetDayOfWeekForDate(int day, int month, int year)
{
      // Leap Year Calculation
    if((fmod(year,4) == 0 && fmod(year,100) != 0) || (fmod(year,400) == 0))
    { leap = 1; }
    else 
    { leap = 0; }

    // Limit results to year 1900-2299 (to save memory)
    while(year > 2299)
    { year = year - 400; }
    while(year < 1900)
    { year = year + 400; }

    // Calculating century
    c = year/100;

    // Calculating two digit year
    yy = fmod(year, 100);

    // Century value based on Table
    if(c == 19) { cTable = 1; }
    if(c == 20) { cTable = 0; }
    if(c == 21) { cTable = 5; }
    if(c == 22) { cTable = 3; }

    // Jan and Feb calculations affected by leap years
    if(month == 1)
    { if(leap == 1) { mTable = 6; }
      else          { mTable = 0; }}
    if(month == 2)
    { if(leap == 1) { mTable = 2; }
      else          { mTable = 3; }}
    // Other months not affected and have set values
    if(month == 10) { mTable = 0; }
    if(month == 8) { mTable = 2; }
    if(month == 3 || month == 11) { mTable = 3; }
    if(month == 4 || month == 7) { mTable = 6; }
    if(month == 5) { mTable = 1; }
    if(month == 6) { mTable = 4; }
    if(month == 9 || month == 12) { mTable = 5; }

    // Enter the data into the formula
    SummedDate = day + mTable + yy + (yy/4) + cTable;
    
    // Find remainder
    DoW = fmod(SummedDate,7);  

   return DoW;
}

int GetNumberOfDaysPerMonth(int month, int year)
{
   if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8|| month == 10|| month == 12)
      return 31;
   else if (month == 2 || month == 4 || month == 6 || month == 9 || month == 11)
      return 30;
   else
   {
     if((fmod(year,4) == 0 && fmod(year,100) != 0) || (fmod(year,400) == 0))
     { 
        return 29;
     }
     else 
     {
       return 28;
     }
   }
}