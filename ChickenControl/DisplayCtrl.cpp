//-----------------------------------------------------------------------------
//  DisplayCtrl.cpp
/// @file
///
/// @brief Contains the task to control the display state
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
#include "DisplayCtrl.h"
#include "DisplayUi.h"

//-----------------------------------------------------------------------------
// Privat definitions, macros and constants
//-----------------------------------------------------------------------------
/// Display on time
#define DISPLAY_ON_TIME_MS 60000

/// Job of the task
enum class DisplayCtrlJob
{
   /// Unknown job
   Unknown = 0,
   /// Job: Switch display on
   SwitchDisplayOn
};

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
// Initializes the display control task
static void InitTaskDisplayCtrl(void);
// Working method to switch the display on
static void DoSwitchDisplayOn(void);
// Callback for display on timer
static void DspOnTmrTimerCallback(TimerHandle_t xTimer);

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------
// Job semaphore of the task
static SemaphoreHandle_t displayCtrlJobSema;
// Current job to process for the task
static DisplayCtrlJob displayCtrlJob = DisplayCtrlJob::Unknown;
// Handle to the timer instance
static TimerHandle_t xDspOnTimer;

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function TaskDisplayCtrl 
///
/// @brief Task function of the display control task
///
/// @param[in] pvParameters - Parameter (not used)
///
/// @see InitTaskDisplayCtrl()
///
//-----------------------------------------------------------------------------
void TaskDisplayCtrl (void * pvParameters)
{
   BaseType_t rc;

   Serial.printf("Start display ctontrol task ...\n");

   InitTaskDisplayCtrl();

   for(;;)
   {
      rc = xSemaphoreTake(displayCtrlJobSema, portMAX_DELAY);
      assert(rc == pdPASS);

      switch (displayCtrlJob)
      {
         case DisplayCtrlJob::SwitchDisplayOn:
         {
            DoSwitchDisplayOn();
         }
         break;

         case DisplayCtrlJob::Unknown:
         default:
            Serial.printf("Unknown display ctrl job!\n");
            break;
      }
   }
}

//-----------------------------------------------------------------------------
//  Function InitTaskDisplayCtrl
///
/// @brief Initializes the display control task
///
/// @see TaskDisplayCtrl()
///
//-----------------------------------------------------------------------------
static void InitTaskDisplayCtrl(void)
{
   displayCtrlJobSema = xSemaphoreCreateBinary();

   xDspOnTimer = xTimerCreate("DspOnTmr",DISPLAY_ON_TIME_MS, pdFALSE, 0, DspOnTmrTimerCallback );
}

//-----------------------------------------------------------------------------
//  Function StartDisplayOn
///
/// @brief Initiates the 'Display on' state
///
/// This function initiates the 'Display on' state.
///
/// @see DoEnableOn()
///
//-----------------------------------------------------------------------------
void StartDisplayOn(void)
{
   displayCtrlJob = DisplayCtrlJob::SwitchDisplayOn;

   xSemaphoreGive(displayCtrlJobSema);
}

//-----------------------------------------------------------------------------
//  Function DoSwitchDisplayOn
///
/// @brief Working method to switch the display on
///
/// This is the working function to switch the display on. Furthermore a timer
/// is started and when the timer has expired the display will be switched off.
///
/// @see StartDisplayOn()
///
//-----------------------------------------------------------------------------
static void DoSwitchDisplayOn(void)
{
   TickType_t xTimeNow;

   Serial.printf("Process switch display on ...\n");

   if (xTimerIsTimerActive(xDspOnTimer) != pdFALSE)
   {
      Serial.printf("Timer is running, stop the timer!\n");
      xTimerStop(xDspOnTimer, 0);
   }

   DisplayOn();

   // display timestamp
   xTimeNow = xTaskGetTickCount();
   Serial.print("Display on timer is start at ");
   Serial.println(xTimeNow);

   xTimerStart( xDspOnTimer, 0 );
}

//-----------------------------------------------------------------------------
//  Function DspOnTmrTimerCallback
///
/// @brief Callback for display on timer
///
/// @param[in] xTimer - Timer handler
///
/// @see DoEnableOn()
///
//-----------------------------------------------------------------------------
static void DspOnTmrTimerCallback(TimerHandle_t xTimer)
{
   TickType_t xTimeNow;
   /* Obtain the current tick count. */
   xTimeNow = xTaskGetTickCount();
   /* Output a string to show the time at which the callback was executed. */
   Serial.print("Display on timer callback executing ");
   Serial.println(xTimeNow);

   DisplayOff();
}