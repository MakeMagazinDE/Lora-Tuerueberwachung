//-----------------------------------------------------------------------------
//  hal_time.c
/// @file
///
/// @brief Hal driver for general timing functions
///
/// @copyright 2017 Hodapp Gmbh & Co. KG - All Rights Reserved
///
/// @status  Released
/// $Date: 2020-02-07 11:26:40 +0100 (Fr, 07 Feb 2020) $
/// $Revision: 3410 $
/// $LastChangedBy: MKoenig $
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include <stdint.h>

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "hal_time.h"

#include "stm32l0xx_hal.h"

//-----------------------------------------------------------------------------
// Private definitions, macros and constants
//-----------------------------------------------------------------------------
/// System clock frequency
#define SYSCLK_HZ                      32000000
/// Number of ticks per micro seconds
#define NR_OF_TICKS_PER_MICROSECOND    (SYSCLK_HZ / 1000000)

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
// Gets the current system tick counter
static uint32_t HAL_TIME_Get_Systick(void);
// Gets the load value for the tick counter
static uint32_t HAL_TIME_Get_Systick_LoadValue(void);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function  HAL_TIME_Get_Systick
///
/// @brief Gets the current system tick counter
///
/// @return  Current system tick counter
///
/// @date    2017-08-15
/// @author  Eike Mueller(TABO)
/// @status  Released
///
//-----------------------------------------------------------------------------
static uint32_t HAL_TIME_Get_Systick(void)
{
  return SysTick->VAL;
}

//-----------------------------------------------------------------------------
//  Function  HAL_TIME_Get_Systick_LoadValue
///
/// @brief Gets the load value for the tick counter
///
/// @return  Load value for the tick counter
///
/// @date    2017-08-15
/// @author  Eike Mueller(TABO)
/// @status  Released
///
//-----------------------------------------------------------------------------
static uint32_t HAL_TIME_Get_Systick_LoadValue(void)
{
  return SysTick->LOAD;
}

//-----------------------------------------------------------------------------
//  Function  HAL_TIME_Delay_MicroSecond
///
/// @brief Waits a delay time, specified in micro seconds
///
/// This function waits a delay time, specified in micro seconds.
/// Note: The wait time is depend on the system clock speed. If this speed is
/// changed (e.g. in CubeMX), please change the value for the defintion SYSCLK_HZ
/// too.
/// MK: Unabhängig vom Sysclk gemacht, nun allerdings abhängig vom Systemtakt 1ms
/// @param[in] delay - Wait time in micro seconds
///
/// @date    2017-08-15
/// @author  Eike Mueller(TABO)
/// @status  Released
///
//-----------------------------------------------------------------------------
void HAL_TIME_Delay_MicroSecond(uint32_t delay)
{
   uint32_t currentTickValue;
   uint32_t triggerTickValue1;
   uint32_t triggerTickValue2;
   uint32_t maxSystickValue;
   uint32_t delayTicks;
   uint8_t ubWait;

   maxSystickValue = HAL_TIME_Get_Systick_LoadValue() + 1;
   delayTicks = delay * (maxSystickValue / 1000);
   currentTickValue = HAL_TIME_Get_Systick();
   triggerTickValue2 = currentTickValue;
   ubWait = 1;
   triggerTickValue1 =  triggerTickValue2 + maxSystickValue - delayTicks;

   if ( triggerTickValue1 >= maxSystickValue )
   {
      triggerTickValue1 -=  maxSystickValue;
      while ( ubWait )
      {
         currentTickValue = HAL_TIME_Get_Systick();
         ubWait = !((currentTickValue <= triggerTickValue1) || (currentTickValue > triggerTickValue2));
      }
   }
   else
   {
      while ( ubWait )
      {
         currentTickValue = HAL_TIME_Get_Systick();
         ubWait = ((currentTickValue <= triggerTickValue2) || (currentTickValue > triggerTickValue1));
      }
   }
}
