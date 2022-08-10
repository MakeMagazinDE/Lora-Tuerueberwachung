//-----------------------------------------------------------------------------
//  KeyCheck.cpp
/// @file
///
/// @brief Contains the task for checking the keys of the system
///
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include "Arduino.h"

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "KeyCheck.h"
#include "DisplayCtrl.h"
#include "GpioDefs.h"

//-----------------------------------------------------------------------------
// Privat definitions, macros and constants
//-----------------------------------------------------------------------------
/// Necessary count of new state to detect a state change
#define NUMBER_COUNTS_FOR_DETECT_KEY_STATE_CHANGE     5

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
// Initializes the key check task
static void InitTaskKeyCheck(void);
// Reads the state of the key 'Display control'
static int ReadDisplayCtrlKey(void);
// Processes the key 'Display control'
static void ProcessDisplayCtrlKey(int keyState);

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------
int currentStateDisplayCtrlKey = 0;

int iChangeDisplayCtrlKeyCounter = 0;

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function TaskKeyCheck 
///
/// @brief Task function of the key check task
///
/// @param[in] pvParameters - Parameter (not used)
///
/// @see InitTaskKeyCheck()
///
//-----------------------------------------------------------------------------
void TaskKeyCheck (void * pvParameters)
{
   int keyState;

   Serial.printf("Start key check task ...\n");

   InitTaskKeyCheck();

   for(;;)
   {
      vTaskDelay(10);

      keyState = ReadDisplayCtrlKey();

      ProcessDisplayCtrlKey(keyState);
   }
}

//-----------------------------------------------------------------------------
//  Function InitTaskKeyCheck
///
/// @brief Initializes the key check task
///
/// @see TaskKeyCheck()
///
//-----------------------------------------------------------------------------
static void InitTaskKeyCheck(void)
{
   pinMode(GPIO39, INPUT);
}

//-----------------------------------------------------------------------------
//  Function ReadDisplayCtrlKey
///
/// @brief Reads the state of the key 'Display control'
///
/// @return State of the key 'Display control'
///
/// @see ProcessDisplayCtrlKey()
///
//-----------------------------------------------------------------------------
static int ReadDisplayCtrlKey(void)
{
   return digitalRead(GPIO39);
}

//-----------------------------------------------------------------------------
//  Function ProcessDisplayCtrlKey
///
/// @brief Processes the key 'Display control'
///
/// @param[in] keyState - Key state
///
/// @see ReadDisplayCtrlKey()
///
//-----------------------------------------------------------------------------
static void ProcessDisplayCtrlKey(int keyState)
{
   if (keyState != currentStateDisplayCtrlKey)
   {
      iChangeDisplayCtrlKeyCounter++;

      if (iChangeDisplayCtrlKeyCounter >= NUMBER_COUNTS_FOR_DETECT_KEY_STATE_CHANGE)
      {
         Serial.printf("New key state: %i!\n", keyState);

         iChangeDisplayCtrlKeyCounter = 0;
         currentStateDisplayCtrlKey = keyState;

         if (1 == keyState)
         {
            StartDisplayOn();
         }
      }
   }
}
