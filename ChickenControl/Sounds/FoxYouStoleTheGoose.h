//-----------------------------------------------------------------------------
//  FoxYouStoleTheGoose.h
/// @file
///
/// @brief Melody for the song 'Fox you stole the goose'
///
/// @status  Draft
/// $Date:$
/// $Revision$
/// $LastChangedBy:$
///
//-----------------------------------------------------------------------------
#ifndef __FOXYOUSTOLETHEGOOSE_H
 #define __FOXYOUSTOLETHEGOOSE_H

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

/// Data for the melody 'Fox you stole the goose'. Note followed by optional
/// change in playing length in 1/4 beats. See documentation for more details
const int8_t PROGMEM FoxYouStoleTheGoose[] = 
{
   NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4,
   NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_G4, NOTE_SILENCE, NOTE_SILENCE, NOTE_SILENCE,
   NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4, NOTE_G4, NOTE_SILENCE, NOTE_SILENCE, NOTE_SILENCE,
   NOTE_G4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4,
   NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_SILENCE,
   NOTE_G4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4,
   NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_SILENCE, NOTE_SILENCE, NOTE_SILENCE,
   NOTE_SILENCE, BEAT_5, SCORE_END
};

#endif //__FOXYOUSTOLETHEGOOSE_H
