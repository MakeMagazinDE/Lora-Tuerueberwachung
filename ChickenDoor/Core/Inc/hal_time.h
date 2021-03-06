//-----------------------------------------------------------------------------
//  hal_time.h
/// @file
///
/// @brief Header for the module hal_time.c
///
/// This is the header for the module hal_time.c.
///
/// @copyright 2017 Hodapp GmbH & co. KG - All Rights Reserved
///
/// @status  Released
/// $Date: 2018-01-26 14:21:39 +0100 (Fr, 26 Jan 2018) $
/// $Revision: 1629 $
/// $LastChangedBy: EMueller $
///
//-----------------------------------------------------------------------------
#ifndef __HAL_TIME_H
 #define __HAL_TIME_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,makros and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Datatypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------
// Waits a delay time, specified in micro seconds
void HAL_TIME_Delay_MicroSecond(uint32_t delay);

#endif //__HAL_TIME_H
