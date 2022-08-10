//-----------------------------------------------------------------------------
//  Sound.cpp
/// @file
///
/// @brief Contains all functions for the 'Play sound'-task
///
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include <arduino.h>

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "PlaySound.h"

#include "MusicDefinitions.h"
#include "XT_DAC_Audio.h"

#include "Sounds\SoundPiano.h"
#include "Sounds\SoundRooster.h"
#include "Sounds\FoxYouStoleTheGoose.h"

//-----------------------------------------------------------------------------
// Private definitions, macros and constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private data types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// External references
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global members of the module
//-----------------------------------------------------------------------------
QueueHandle_t xPlaySoundQueue;

/// Create an object of type XT_Wav_Class that is used by the dac audio class (below), passing wav data as parameter.
XT_Wav_Class Piano(piano);

/// Create an object of type XT_Wav_Class that is used by the dac audio class (below), passing wav data as parameter. 
XT_Wav_Class Rooster(rooster);

/// Create the main player class object (Use GPIO 25, one of the 2 DAC pins and timer 0)
XT_DAC_Audio_Class DacAudio(26,0);

/// Music score object, handles tunes. Pass Music Data,Tempo, Instrument 
XT_MusicScore_Class Music((int8_t*)FoxYouStoleTheGoose, TEMPO_ALLEGRO, INSTRUMENT_HARPSICHORD); 

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
void PlaySound(unsigned long sound);
void PlaySoundPiano(void);
void PlaySoundRooster(void);
void PlaySoundFoxYouStoleTheGoose(void);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

void TaskPlaySound (void * pvParameters)
{
   unsigned long sound;

   for(;;)
   {
      if( xQueueReceive( xPlaySoundQueue,
                        &sound,
                        ( TickType_t ) portMAX_DELAY ) == pdPASS )
      {
         Serial.printf("Play new sound: %i!\n", sound);

         PlaySound(sound);
      }
   }
}

void PlaySound(unsigned long sound)
{
   switch (sound)
   {
      case SOUND_PIANO: PlaySoundPiano(); break;
      case SOUND_ROOSTER: PlaySoundRooster(); break;
      case SOUND_FOX_YOU_STOLE_THE_GOOSE: PlaySoundFoxYouStoleTheGoose(); break;
      default: break;
   }
}

void PlaySoundPiano(void)
{
   DacAudio.Play(&Piano);

   for(int i=0; i < 400; i++)
   {       
      vTaskDelay(10);
        
      DacAudio.FillBuffer();
   }
}

void PlaySoundRooster(void)
{
   DacAudio.Play(&Rooster);

   for(int i=0; i < 200; i++)
   {       
     vTaskDelay(10);

     DacAudio.FillBuffer();
   }
}

void PlaySoundFoxYouStoleTheGoose(void)
{
   DacAudio.Play(&Music);

   for(int i=0; i < 1500; i++)
   {       
      vTaskDelay(10);

      DacAudio.FillBuffer();
   }
}

void QueueInSoundToPlay(unsigned long sound)
{
   if( xQueueSend( xPlaySoundQueue,
                 ( void * ) &sound,
                 ( TickType_t ) 10 ) != pdPASS )
   {
      Serial.printf("Send play sound queue failed!\n");
   }
}


void CreatePlaySoundQueue(void)
{
   xPlaySoundQueue = xQueueCreate( 10, sizeof( unsigned long ) );

   if (xPlaySoundQueue == NULL)
   {
      Serial.printf("Creation of play sound queue failed!\n");
   }
}