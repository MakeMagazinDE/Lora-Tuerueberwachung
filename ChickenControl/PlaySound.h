//-----------------------------------------------------------------------------
//  PlaySound.h
/// @file
///
/// @brief Header for the module PlaySound.cpp
///
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
///
//-----------------------------------------------------------------------------
#ifndef __PLAYSOUND_H
 #define __PLAYSOUND_H

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions ,macros and constants
//-----------------------------------------------------------------------------
// Sound id
/// Piano sound (wav file)
#define SOUND_PIANO     0
/// Rooster sound (wav file)
#define SOUND_ROOSTER   1
/// Song 'Fox you stole the goose' (synthetic)
#define SOUND_FOX_YOU_STOLE_THE_GOOSE 2

//-----------------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes of public functions
//-----------------------------------------------------------------------------

void TaskPlaySound( void *pvParameters );
void CreatePlaySoundQueue(void);
void QueueInSoundToPlay(unsigned long sound);

#endif //__PLAYSOUND_H
