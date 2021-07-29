 //-----------------------------------------------------------------------------
//  ChickenControl.ino
/// @file
///
/// @brief Central file for chicken control sketch
///
/// @status  Draft
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include "Arduino.h"

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "GpioDefs.h"

#include "PlaySound.h"
#include "DisplayUi.h"
#include "TimeLoc.h"
#include "Dusk2DawnLoc.h"
#include "ChickenControlData.h"
#include "LoRa.h"

//-----------------------------------------------------------------------------
// Private definitions, macros and constants
//-----------------------------------------------------------------------------

#if CONFIG_FREERTOS_UNICORE
 /// Core for running the arduino tasks
 #define ARDUINO_RUNNING_CORE          0
#else
 /// Core for running the arduino tasks
 #define ARDUINO_RUNNING_CORE          1
#endif

/// Timeout value in seconds for detecting that contact with the chicken house
/// has been lost
#define TIMEOUT_CONTACT_CHICKEN_HOUSE  50

/// Reminder interval in minutes (the reminder melody is played in intervals
/// until the chicken house door is closed)
#define REMINDER_INTERVAL_MIN          5

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------
/// Semaphore to protect the chicken house data
extern SemaphoreHandle_t semaChickenHouseData;

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------
/// Current last reminder value
int dayMinutsLastReminder = 0;

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
// Builds some general things of the user interface
void BuildUserInterface(void);
// Calculates the 'Close the door' - reminder
void CalculateReminder(void);
// Initializes the used gpio pins
void InitGpio(void);
// Initializes the task system
void InitTaskSystem(void);
// Task for cylic calculation of the 'Close the door' - reminder
void TaskCalcReminder(void * pvParameters);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Funktion setup
///
/// @brief Setup function of the sketch
///
/// @see loop()
///
/// @date    2021-01-10
/// @author  Eike Mueller(TABO)
/// @status  Entwurf
///
//-----------------------------------------------------------------------------
void setup()
{
   Serial.begin(115200);
   Serial.println();

   InitGpio();

   InitDisplayUi();

   GetTimeFromNTP();

   delay(2000);

   InitTaskSystem();

   QueueInSoundToPlay(SOUND_PIANO);

   // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

//-----------------------------------------------------------------------------
//  Funktion loop
///
/// @brief Loop function of the sketch
///
/// @see setup()
///
/// @date    2021-01-10
/// @author  Eike Mueller
/// @status  Entwurf
///
//-----------------------------------------------------------------------------
void loop()
{
   // nothing to do, we have tasks in FreeRTOS
}

//-----------------------------------------------------------------------------
//  Function InitGpio
///
/// @brief Initializes the used gpio pins
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void InitGpio(void)
{
   // Initialize the gpio pins
   pinMode(RED_LED, OUTPUT);
   pinMode(GREEN_LED, OUTPUT);
   pinMode(26, OUTPUT);
}

//-----------------------------------------------------------------------------
//  Function InitTaskSystem
///
/// @brief Initializes the task system
///
/// This function initializes the task system. A priority value of 0 is being
/// the lowest.
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void InitTaskSystem(void)
{
   CreatePlaySoundQueue();

   xTaskCreatePinnedToCore(
   TaskUpdateUi
   ,  "tUpdateUi" 
   ,  8192
   ,  NULL
   ,  3
   ,  NULL 
   ,  ARDUINO_RUNNING_CORE);

   xTaskCreatePinnedToCore(
   TaskPlaySound
   ,  "tPlaySnd"
   ,  8192
   ,  NULL
   ,  2
   ,  NULL 
   ,  ARDUINO_RUNNING_CORE);

   xTaskCreatePinnedToCore(
   TaskLoRaScan
   ,  "tLoRaScan"
   ,  8192
   ,  NULL
   ,  5
   ,  NULL 
   ,  ARDUINO_RUNNING_CORE);

   xTaskCreatePinnedToCore(
   TaskCalcReminder
   ,  "tCalcRem"
   ,  8192
   ,  NULL
   ,  3
   ,  NULL 
   ,  ARDUINO_RUNNING_CORE);

   semaChickenHouseData = xSemaphoreCreateMutex();
}

//-----------------------------------------------------------------------------
//  Function TaskCalcReminder
///
/// @brief Task for cylic calculation of the 'Close the door' - reminder
///
/// @param[in] pvParameters - Task parameter
///
/// @see CalculateReminder()
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void TaskCalcReminder(void * pvParameters)
{
   for(;;)
   {
      vTaskDelay(1000);

      CalculateReminder();

      BuildUserInterface();
   }
}

//-----------------------------------------------------------------------------
//  Function CalculateReminder
///
/// @brief Calculates the 'Close the door' - reminder
///
/// @see TaskCalcReminder()
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void CalculateReminder(void)
{
   struct tm timeinfo;
   int dayMinuts;

   if (!GetChickenHouseConnected() || GetChickenHouseDoorState())
      return;

   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return;
   }

   int sunset = GetSunsetForToday();
   int sunrise = GetSunriseForToday();

   dayMinuts = (timeinfo.tm_hour * 60) + timeinfo.tm_min;

   if ((dayMinuts + 30) > sunset || dayMinuts < sunrise)
   {
      if (dayMinutsLastReminder == 0 || dayMinuts - dayMinutsLastReminder > REMINDER_INTERVAL_MIN || dayMinuts < dayMinutsLastReminder)
      {
         QueueInSoundToPlay(SOUND_FOX_YOU_STOLE_THE_GOOSE);
         dayMinutsLastReminder = dayMinuts;
      }
   }
}

//-----------------------------------------------------------------------------
//  Function BuildUserInterface
///
/// @brief Builds some general things of the user interface
///
/// @see TaskCalcReminder()
///
/// @date    2021-01-15
/// @author  Eike Mueller
/// @status  Draft
///
//-----------------------------------------------------------------------------
void BuildUserInterface(void)
{
   uint8_t stateChickenHouseDoor;
   struct tm timestampLastChickenHouseContact;
   struct tm timeinfo;
   time_t timeinfoSec;
   time_t timestampLastChickenHouseContactSec;

   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return;
   }

   if (GetChickenHouseConnected())
   {
      GetTimestampLastChickenHouseContact(&timestampLastChickenHouseContact);

      timestampLastChickenHouseContactSec = mktime(&timestampLastChickenHouseContact);

      timeinfoSec = mktime(&timeinfo);

      stateChickenHouseDoor = GetChickenHouseDoorState();

      if (stateChickenHouseDoor == 0 || (timeinfoSec - timestampLastChickenHouseContactSec) > TIMEOUT_CONTACT_CHICKEN_HOUSE)
      {
         if ((timeinfoSec - timestampLastChickenHouseContactSec) > TIMEOUT_CONTACT_CHICKEN_HOUSE)
         {
            SetChickenHouseData(0, 0, 0);

            SetChickenHouseConnected(false);
         }
         else
         {
         }

         EnableStatusFrame();

         digitalWrite(RED_LED, HIGH);
         digitalWrite(GREEN_LED, LOW);
      }
      else
      {
         DisableStatusFrame();

         digitalWrite(RED_LED, LOW);
         digitalWrite(GREEN_LED, HIGH);
      }
   }
   else
   {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
   }
}
