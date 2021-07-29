//-----------------------------------------------------------------------------
//  GpioDefs.h
/// @file
///
/// @brief <Please fill out with the short description!>
///
/// <Please fill out with the extended description!>
///
/// @see <Cross reference to other classes ...>
///
/// @copyright 2013-2021 Hodapp GmbH & Co. KG - All Rights Reserved
///
/// @status  Draft
/// $Date:$
/// $Revision$
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
#ifndef __GPIODEFS_H
 #define __GPIODEFS_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,macros and constants
//-----------------------------------------------------------------------------
#define GPIO0           0
#define GPIO16          16
#define GPIO2           2
#define GPIO5           5
#define GPIO4           4
#define GPIO25          25

#define HSPI_MISO       12
#define HSPI_MOSI       13
#define HSPI_SCLK       14
#define HSPI_SS         15

#define OLED_SDA        GPIO5
#define OLED_SCL        GPIO4

#define OLED_ADDRESS    0x3c

#define RED_LED         GPIO0
#define GREEN_LED       GPIO2

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------



#endif //__GPIODEFS_H
