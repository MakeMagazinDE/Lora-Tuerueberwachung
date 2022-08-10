//-----------------------------------------------------------------------------
//  DisplayUi.h
/// @file
///
/// @brief Header for the module DisplayUi.cpp
///
/// $Date: $
/// $Revision: $
/// $LastChangedBy: $
///
//-----------------------------------------------------------------------------
#ifndef __DISPLAYUI_H
 #define __DISPLAYUI_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,macros and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------

// Initializes the display user interface
void InitDisplayUi(void);
// Enables the status frame
void EnableStatusFrame(void);
// Disables the status frame
void DisableStatusFrame(void);
// Switches the display on
void DisplayOn(void);
// Switches the display off
void DisplayOff(void);

#endif //__DISPLAYUI_H
