//-----------------------------------------------------------------------------
//  DisplayUi.cpp
/// @file
///
/// @brief Contains all function to control the user interface output to the oled display
///
/// @status  Draft
/// $Date:$
/// $Revision:$
/// $LastChangedBy:$
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard include files
//-----------------------------------------------------------------------------
#include <TimeLib.h>

//-----------------------------------------------------------------------------
// Project specific include files
//-----------------------------------------------------------------------------
#include "DisplayUi.h"
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "GpioDefs.h"
#include "Utilities.h"
#include "Dusk2DawnLoc.h"
#include "ChickenControlData.h"
#include "DisplayCtrl.h"

#include "Images/ChickenSymbol.h"
#include "Images/BatterySymbol.h"
#include "Images/ActiveSymbol.h"
#include "Images/InactiveSymbol.h"

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
// Initialize the OLED display using Wire library
/// Controller object for the SSD1306 controller
SSD1306Wire  display(OLED_ADDRESS, OLED_SDA, OLED_SCL);
/// Oled display user interface object
OLEDDisplayUi ui ( &display );

/// Screen width
int screenW = 128;
/// Screen height
int screenH = 64;
/// Center of the screen x-axis
int screenCenterX = screenW/2;
/// Center of the screen y-axis
int screenCenterY = ((screenH-16)/2)+16;   // top yellow part is 16 px height
/// Flag: Switch the display on in the task main loop
bool switchDisplayOn = false;
/// Flag: Switch the display off in the task main loop
bool swichtDisplayOff = false;

//-----------------------------------------------------------------------------
// Prototypes of private functions
//-----------------------------------------------------------------------------
// Draws the start frame of the application
static void startFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws the digital clock frame of the application (current system time)
static void digitalClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws the sunrise frame of the application
static void sunriseFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws the sunset frame of the application
static void sunsetFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws an rectangle frame in the complete display
static void rectangleFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws the battery voltage frame
static void batteryVoltageFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws the temperature frame
static void temperatureFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Draws the status frame (error case)
static void statusFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
// Dummy overlay function
static void clockOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
// Draws the battery symbol with the current battery state
static void displayBatterySymbol(OLEDDisplay *display);
// Draws the rssi value
static void displayRssi(OLEDDisplay *display);
// Process the display on/off state
static void ProcessDisplayOnOff(void);

/// Frame list a start of the system
FrameCallback startframes[] = { startFrame };

/// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { clockOverlay };
/// Overlay count
int overlaysCount = 1;

/// This array keeps function pointers to all frames. Frames are the single views that slide in.
FrameCallback frames[] = { digitalClockFrame, sunriseFrame, sunsetFrame, batteryVoltageFrame, temperatureFrame, statusFrame };

/// Number of frames to slide
int frameCount = 6;

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Function InitDisplayUi
///
/// @brief Initializes the display user interface
///
//-----------------------------------------------------------------------------
void InitDisplayUi(void)
{
   // Add frames for the ui
   ui.setFrames(startframes, 1);

   // Add overlays
   ui.setOverlays(overlays, overlaysCount);

   // Initialising the UI will init the display too.
   ui.init();

   display.flipScreenVertically();

   ui.update();
}

//-----------------------------------------------------------------------------
//  Function startFrame
///
/// @brief Draws the start frame of the application
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - <parameter description>
///
/// @param[in] y - <parameter description>
///
//-----------------------------------------------------------------------------
static void startFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   display->drawXbm(0, 0, CHICKEN_SYMBOL_WIDTH, CHICKEN_SYMBOL_HEIGHT, ChickenSymbol);

   String datenow = "Chicken";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(96, screenCenterY - 35, datenow );

   String timenow = "Control";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(96 , screenCenterY - 15, timenow );

   String version = "V1.2";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(96 , screenCenterY + 5, version );
}

//-----------------------------------------------------------------------------
//  Function digitalClockFrame
///
/// @brief Draws the digital clock frame of the application (current system time)
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void digitalClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   struct tm timeinfo;

   if(!getLocalTime(&timeinfo))
   {
      Serial.println("Failed to obtain time");
      return;
   }

   String timenow = String(timeinfo.tm_hour)+":"+twoDigits(timeinfo.tm_min)+":"+twoDigits(timeinfo.tm_sec);
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_24);
   display->drawString(screenCenterX + x , screenCenterY + y, timenow );

   String datenow = twoDigits(timeinfo.tm_mday)+"."+twoDigits(timeinfo.tm_mon + 1)+"."+String(timeinfo.tm_year + 1900);
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_24);
   display->drawString(screenCenterX + x , screenCenterY - 26, datenow );

   displayBatterySymbol(display);
   displayRssi(display);
}

//-----------------------------------------------------------------------------
//  Function  sunriseFrame
///
/// @brief Draws the sunrise frame of the application
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void sunriseFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   int sunrise  = GetSunriseForToday();

   String timenow = twoDigits(sunrise / 60)+":"+twoDigits(sunrise % 60);
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_24);
   display->drawString(screenCenterX + x , screenCenterY + y, timenow );

   String datenow = "Sonnenaufgang";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(screenCenterX + x , screenCenterY - 26, datenow );

   displayBatterySymbol(display);
   displayRssi(display);
}

//-----------------------------------------------------------------------------
//  Function  sunsetFrame
///
/// @brief Draws the sunset frame of the application
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void sunsetFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   int sunset  = GetSunsetForToday();

   String timenow = twoDigits(sunset / 60)+":"+twoDigits(sunset % 60);
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_24);
   display->drawString(screenCenterX + x , screenCenterY + y, timenow );

   String datenow = "Sonnenuntergang";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(screenCenterX + x , screenCenterY - 26, datenow );

   displayBatterySymbol(display);
   displayRssi(display);
}

//-----------------------------------------------------------------------------
//  Function  displayBatterySymbol
///
/// @brief Draws the battery symbol with the current battery state
///
/// @param[in] *display - Oled display user interface object
///
//-----------------------------------------------------------------------------
static void displayBatterySymbol(OLEDDisplay *display)
{
   uint8_t batteryFuelPct;

   batteryFuelPct = GetChickenHouseBatteryFuelPct();

   if (batteryFuelPct >= 80)
   {
      display->drawXbm(0, 0, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT, Battery_80_100);
   }
   else if (batteryFuelPct >= 60)
   {
      display->drawXbm(0, 0, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT, Battery_60_80);
   }
   else if (batteryFuelPct >= 40)
   {
      display->drawXbm(0, 0, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT, Battery_40_60);
   }
   else if (batteryFuelPct >= 20)
   {
      display->drawXbm(0, 0, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT, Battery_20_40);
   }
   else if (batteryFuelPct >= 10)
   {
      display->drawXbm(0, 0, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT, Battery_10_20);
   }
   else
   {
      display->drawXbm(0, 0, BATTERY_SYMBOL_WIDTH, BATTERY_SYMBOL_HEIGHT, Battery_0_10_Low);
   }
}

//-----------------------------------------------------------------------------
//  Function  displayRssi
///
/// @brief Draws the rssi value
///
/// @param[in] *display - Oled display user interface object
///
//-----------------------------------------------------------------------------
static void displayRssi(OLEDDisplay *display)
{
   String rssi = String(GetLastRssi());
   display->setTextAlignment(TEXT_ALIGN_RIGHT);
   display->setFont(ArialMT_Plain_10);
   display->drawString(127 , 0, rssi );
}

//-----------------------------------------------------------------------------
//  Function  rectangleFrame
///
/// @brief Draws an rectangle frame in the complete display
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void rectangleFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   display->drawRect(0,0,128,64);
   display->fillRect(0,0,128,64);
}

//-----------------------------------------------------------------------------
//  Function  statusFrame
///
/// @brief Draws the status frame (error case)
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void statusFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   if (!GetChickenHouseConnected())
   {
      String text = "Kein Kontakt!";
      display->setTextAlignment(TEXT_ALIGN_CENTER);
      display->setFont(ArialMT_Plain_16);
      display->drawString(screenCenterX + x , screenCenterY + y, text );
   }
   else if (GetChickenHouseDoorState() == 0)
   {
      String text = "Tür offen!";
      display->setTextAlignment(TEXT_ALIGN_CENTER);
      display->setFont(ArialMT_Plain_16);
      display->drawString(screenCenterX + x , screenCenterY + y, text );
   }

   displayBatterySymbol(display);
   displayRssi(display);
}

//-----------------------------------------------------------------------------
//  Function  batteryVoltageFrame
///
/// @brief Draws the battery voltage frame
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void batteryVoltageFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   char buff[10];

   dtostrf(GetChickenHouseBatteryVoltage(), 1, 3, buff);  //4 is mininum width, 6 is precision
   String batVoltage = buff + String("V");
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_24);
   display->drawString(screenCenterX + x , screenCenterY + y, batVoltage );

   String batVoltageText = "Batteriespannung";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(screenCenterX + x , screenCenterY - 26, batVoltageText );

   displayBatterySymbol(display);
   displayRssi(display);
}

//-----------------------------------------------------------------------------
//  Function  temperatureFrame
///
/// @brief Draws the temperature frame
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
/// @param[in] x - x Position
///
/// @param[in] y - y Position
///
//-----------------------------------------------------------------------------
static void temperatureFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
   char buff[10];

   String temperatureString = String(GetChickenHouseTemperature())+" °C";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_24);
   display->drawString(screenCenterX + x , screenCenterY + y, temperatureString );

   String batVoltageText = "Temperatur";
   display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   display->drawString(screenCenterX + x , screenCenterY - 26, batVoltageText );

   displayBatterySymbol(display);
   displayRssi(display);
}

/* Overlays */
//-----------------------------------------------------------------------------
//  Function  clockOverlay
///
/// @brief Dummy overlay function
///
/// @param[in,out] *display - Oled display user interface object
///
/// @param[in,out] state - Ui state
///
//-----------------------------------------------------------------------------
static void clockOverlay(OLEDDisplay *display, OLEDDisplayUiState* state)
{
}

//-----------------------------------------------------------------------------
//  Function  TaskUpdateUi
///
/// @brief Task function of the update ui task
///
/// @param[in] *pvParameters - Task parameters (not used)
///
//-----------------------------------------------------------------------------
void TaskUpdateUi( void *pvParameters )
{
   display.displayOff();

   // The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
   ui.setTargetFPS(60);

	// Customize the active and inactive symbol
   ui.setActiveSymbol(activeSymbol);
   ui.setInactiveSymbol(inactiveSymbol);

   // You can change this to
   // TOP, LEFT, BOTTOM, RIGHT
   ui.setIndicatorPosition(TOP);

   // Defines where the first frame is located in the bar.
   ui.setIndicatorDirection(LEFT_RIGHT);

   // You can change the transition that is used
   // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
   ui.setFrameAnimation(SLIDE_LEFT);

   // Add frames
   ui.setFrames(frames, frameCount);

   // Add overlays
   ui.setOverlays(overlays, overlaysCount);

   // Initialising the UI will init the display too.
   ui.init();

   display.flipScreenVertically();

   StartDisplayOn();

   for(;;)
   {
      vTaskDelay(100);

      ProcessDisplayOnOff();

      int remainingTimeBudget = ui.update();

      if (remainingTimeBudget > 0)
      {
         // You can do some work here
         // Don't do stuff if you are below your
         // time budget.
         vTaskDelay(remainingTimeBudget);
      }
   }
}

//-----------------------------------------------------------------------------
//  Function  EnableStatusFrame
///
/// @brief Enables the status frame
///
/// @see DisableStatusFrame()
///
//-----------------------------------------------------------------------------
void EnableStatusFrame(void)
{
   if (frameCount != 6)
   {
      frameCount = 6;      

      // Add frames
      ui.setFrames(frames, frameCount);

      // Initialising the UI will init the display too.
      ui.init();

      display.flipScreenVertically();

      ui.transitionToFrame(4);
   }
}

//-----------------------------------------------------------------------------
//  Function  DisableStatusFrame
///
/// @brief Disables the status frame
///
/// @see EnableStatusFrame()
///
//-----------------------------------------------------------------------------
void DisableStatusFrame(void)
{
   if (frameCount != 5)
   {
      frameCount = 5;
      // Add frames
      ui.setFrames(frames, frameCount);

      // Initialising the UI will init the display too.
      ui.init();

      display.flipScreenVertically();
   }
}

//-----------------------------------------------------------------------------
//  Function DisplayOn
///
/// @brief Switches the display on
///
/// @see DisplayOff()
///
//-----------------------------------------------------------------------------
void DisplayOn(void)
{
   switchDisplayOn = true;
}

//-----------------------------------------------------------------------------
//  Function DisplayOff
///
/// @brief Switches the display off
///
/// @see DisplayOn()
///
//-----------------------------------------------------------------------------
void DisplayOff(void)
{
   swichtDisplayOff = true;
}

//-----------------------------------------------------------------------------
//  Function ProcessDisplayOnOff
///
/// @brief Processes the display on/off-request from outside the task
///
/// @see DisplayOn(), DisplayOff()
///
//-----------------------------------------------------------------------------
static void ProcessDisplayOnOff(void)
{
   if (switchDisplayOn)
   {
      display.displayOn();
      switchDisplayOn = false;
   }

   if (swichtDisplayOff)
   {
      display.displayOff();
      swichtDisplayOff = false;
   }
}