//-----------------------------------------------------------------------------
//  Utilities.cpp
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
#include <Arduino.h>

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "Utilities.h"

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

// utility function for digital clock display: prints leading 0
String twoDigits(int digits)
{
   if(digits < 10)
   {
      String i = '0'+String(digits);
      return i;
   }
   else
   {
      return String(digits);
   }
}