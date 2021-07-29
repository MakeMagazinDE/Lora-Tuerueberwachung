//-----------------------------------------------------------------------------
//  BatterySymbol.h
/// @file
///
/// @brief Data definitions for the battery symbols
///
/// @status  Draft
/// $Date: 2021-01-14 20:28:56 +0100 (Do, 14 Jan 2021) $
/// $Revision: 23 $
/// $LastChangedBy: EMueller $
///
//-----------------------------------------------------------------------------
#ifndef __BATTERYSYMBOL_H
 #define __BATTERYSYMBOL_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,macros and constants
//-----------------------------------------------------------------------------
/// Width of all battery symbols
#define BATTERY_SYMBOL_WIDTH     15
/// Height of all battery symbols
#define BATTERY_SYMBOL_HEIGHT    10

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------
/// Data for the battery symbol for fuel level 0..10%
const uint8_t Battery_0_10_Low[] PROGMEM = 
{
   0xFE, 0x7F, 0x02, 0x40, 0x02, 0x40, 0x03, 0x40, 0x03, 0x40, 0x03, 0x40, 0x03, 0x40, 0x02, 0x40,
   0x02, 0x40, 0xFE, 0x7F
};

/// Data for the battery symbol for fuel level 10..20%
const uint8_t Battery_10_20[] PROGMEM = 
{
   0xFE, 0x7F, 0x02, 0x40, 0x02, 0x58, 0x03, 0x58, 0x03, 0x58, 0x03, 0x58, 0x03, 0x58, 0x02, 0x58,
   0x02, 0x40, 0xFE, 0x7F
};

/// Data for the battery symbol for fuel level 20..40%
const uint8_t Battery_20_40[] PROGMEM = 
{
   0xFE, 0x7F, 0x02, 0x40, 0x02, 0x5E, 0x03, 0x5E, 0x03, 0x5E, 0x03, 0x5E, 0x03, 0x5E, 0x02, 0x5E,
   0x02, 0x40, 0xFE, 0x7F
};

/// Data for the battery symbol for fuel level 40..60%
const uint8_t Battery_40_60[] PROGMEM = 
{
   0xFE, 0x7F, 0x02, 0x40, 0x82, 0x5F, 0x83, 0x5F, 0x83, 0x5F, 0x83, 0x5F, 0x83, 0x5F, 0x82, 0x5F,
   0x02, 0x40, 0xFE, 0x7F
};
    
/// Data for the battery symbol for fuel level 60..80%
const uint8_t Battery_60_80[] PROGMEM = 
{
   0xFE, 0x7F, 0x02, 0x40, 0xE2, 0x5F, 0xE3, 0x5F, 0xE3, 0x5F, 0xE3, 0x5F, 0xE3, 0x5F, 0xE2, 0x5F,
   0x02, 0x40, 0xFE, 0x7F
};

/// Data for the battery symbol for fuel level 80..100%
const uint8_t Battery_80_100[] PROGMEM = 
{
   0xFE, 0x7F, 0x02, 0x40, 0xFA, 0x5F, 0xFB, 0x5F, 0xFB, 0x5F, 0xFB, 0x5F, 0xFB, 0x5F, 0xFA, 0x5F,
   0x02, 0x40, 0xFE, 0x7F
};

#endif //__BATTERYSYMBOL_H
