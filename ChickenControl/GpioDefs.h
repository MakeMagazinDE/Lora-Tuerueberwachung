//-----------------------------------------------------------------------------
//  GpioDefs.h
/// @file
///
/// @brief Contains all GPIO definitions
///
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
///
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
/// ESP32 Gpio pin 0
#define GPIO0           0
/// ESP32 Gpio pin 16
#define GPIO16          16
/// ESP32 Gpio pin 2
#define GPIO2           2
/// ESP32 Gpio pin 5
#define GPIO5           5
/// ESP32 Gpio pin 4
#define GPIO4           4
/// ESP32 Gpio pin 25
#define GPIO25          25
/// ESP32 Gpio pin 39
#define GPIO39          39

// Spi for LORA module connection
/// ESP32 Pin MISO
#define HSPI_MISO       12
/// ESP32 Pin MOSI
#define HSPI_MOSI       13
/// ESP32 Pin SCLK
#define HSPI_SCLK       14
/// ESP32 Pin SS
#define HSPI_SS         15

// I2C for oled
/// ESP32 Pin SDA
#define OLED_SDA        GPIO5
/// ESP32 Pin SCL
#define OLED_SCL        GPIO4
/// I2C address oled module
#define OLED_ADDRESS    0x3c

/// Red led
#define RED_LED         GPIO0
/// Green led
#define GREEN_LED       GPIO2

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------

#endif //__GPIODEFS_H
