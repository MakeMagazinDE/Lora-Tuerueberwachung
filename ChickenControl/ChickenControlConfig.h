//-----------------------------------------------------------------------------
//  ChickenControlConfig.h
/// @file
///
/// @brief Configurtaion file for the chicken control application
///
/// @status  Draft
/// $Date:$
/// $Revision$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------
#ifndef __CHICKENCONTROLCONFIG_H
 #define __CHICKENCONTROLCONFIG_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,macros and constants
//-----------------------------------------------------------------------------
#define GAMSTAEDT

#ifdef GRAEVENWIESBACH
/// Ssid for the wlan access
#define WLAN_SSID                "Hobbithoehle"
/// Password for the wlan access
#define WLAN_PW                  "4705028430432547"
#else
/// Ssid for the wlan access
#define WLAN_SSID                "FRITZ!Box 7272"
/// Password for the wlan access
#define WLAN_PW                  "44579886255681963683"
#endif

/// Latitude value for calculating the sunset and sunrise
#define LATITUDE                 50.95
/// Longitude value for calculating the sunset and sunrise
#define LONGITUDE                10.8667
/// Timezone for calculating the sunset and sunrise
#define TIMEZONE                 2

/// Battery voltage representing 100% battery fuel
#define BATTERY_VOLTAGE_100_PCT  3.1f
/// Battery voltage representing 0% battery fuel
#define BATTERY_VOLTAGE_0_PCT    2.0f

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------
/// Use the NTP server to get the current time
#define USE_NTP_SERVER

#endif //__CHICKENCONTROLCONFIG_H
