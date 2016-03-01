/*
 * ScreensManager.h
 *
 * Created: 4/11/2014 7:59:39 AM
 *  Author: Sherif
 */ 


#ifndef SCREENSMANAGER_H_
#define SCREENSMANAGER_H_

#include "Marlin.h"
#include "FT_Breakout_4DLCD_FT843.h"

#include "FT843_SplashScreen.h"
#include "FT843_HomeScreen.h"
#include "FT843_SdCardScreen.h"
#include "FT843_Print_HeatingScreen.h"
#include "FT843_PrintingScreen.h"
#include "FT843_PreheatScreen.h"
#include "FT843_StartupScreen.h"
#include "FT843_FilamentScreen.h"
#include "FT843_UtilitiesScreen.h"
#include "FT843_SettingsScreen.h"

SplashScreen Splash;
HomeScreen Home;
SdCardScreen SdCard;
PrintHeatingScreen PrintHeating;
PrintingScreen printingScreen;
PreheatScreen preheatScreen;
StartupScreen startupScreen;
FilamentScreen filamentScreen;
UtilitiesScreen utilitiesScreen;
SettingsScreen settingsScreen;

ScreenBase* activeScreen;

uint8_t eventFlag;
uint8_t tag;
uint8_t screenWorked;

uint32_t screenRefreshTime;

void smDisplayScreen(uint8_t screenID);

int8_t isFileSystemVolumeInfo()
{
	int8_t ret = strcmp_P(card.longFilename, PSTR("System Volume Information"));
	
	// If string compare not equal 0 (Meaning strings aren't equal) then set as 0, else if it's == 0 (then strings are equal) and set to 1
	ret = ret != 0 ? 0 : 1; 
	return ret;
}

void ScreensManagerInit()
{
	Splash.init();
	screenRefreshTime = 0;
	clearHeatingFlag();
	
	screenWorked = 0;
	
	#if ROBO_VERSION == ROBO_R2
	DDRG |= 1 << 4;
	PORTG |= 1 << 4;
	#endif
	
	activeScreen = (ScreenBase*)&Splash;
	if(activeScreen->BootupConfigure())
	{
		screenWorked = 1;
		Splash.clearScreen();
		Splash.clearScreen();
		Splash.animate();
		//Splash.freeMem();
		#if ROBO_VERSION == ROBO_R2 
		if (getStartUpSequence() != 'R')
		{
			smDisplayScreen(SCREEN_STARTUP);
		}
		else
		#endif
		{
			smDisplayScreen(SCREEN_HOME);
		}
	}
	else
	{
		//card.initsd();
		//if (!card.cardOK)
		//{
			//return;
		//}
		//
		//int filesCount = card.getnrfilenames();
		//for (int i = 0; i < filesCount; i++)
		//{
			//card.getfilename(i);
			//if (!isFileSystemVolumeInfo())
			//{
				//SERIAL_ECHO_START;
				//SERIAL_ECHOLN(card.longFilename);
			//}
		//}
	}
}

uint16_t tempCounter = 0;

void ScreensManagerLoop()
{
	//return;
	
	//if (!screenWorked)
	//{
		//return;
	//}
	
	tag = activeScreen->read_tags(&eventFlag);
	//SERIAL_ECHO_START;
	//SERIAL_ECHO("Inside screenManagersLoop. Tag: ");
	//MYSERIAL.println(tag, DEC);
	if (tag != 0)
	{
		activeScreen->tagPressed(tag);
	}
	else
	{		
		if (millis() >= (screenRefreshTime + 30))
		{
			screenRefreshTime = millis();
			
			//SERIAL_ECHO_START;
			//SERIAL_ECHOLNPGM("Before processing screen (IE refreshing)");
			uint8_t ret = activeScreen->process();
			if (ret != 0)
			{
				smDisplayScreen(ret);
			}
			
			//SERIAL_ECHO_START;
			//SERIAL_ECHOLNPGM("After processing screen");
		}
	}
	
	
	if (eventFlag)
	{
		uint8_t screenID = activeScreen->tagReleased(tag);
		smDisplayScreen(screenID);
	}
}

void smDisplayScreen(uint8_t screenID)
{
	switch(screenID)
	{
		case SCREEN_SPLASH: // Do nothing
			break;
		case SCREEN_HOME:
			activeScreen->clearScreen();
			activeScreen->freeMem();
			Home.init();
			activeScreen = (ScreenBase*)&Home;
			activeScreen->displayControls(255);
			break;
		case SCREEN_PRINT_SDCARD:
			activeScreen->clearScreen();
			activeScreen->freeMem();
			SdCard.init();
			activeScreen = (ScreenBase*)&SdCard;
			activeScreen->displayControls(255);
			break;
		case SCREEN_PRINT_HEATING:
			activeScreen->clearScreen();
			PrintHeating.init();
			activeScreen->freeMem();
			activeScreen = (ScreenBase*)&PrintHeating;
			activeScreen->displayControls(255);
			//smDisplayScreen(SCREEN_PRINTING);
			break;
		case SCREEN_PRINTING:
			activeScreen->clearScreen();
			printingScreen.init();
			activeScreen->freeMem();
			activeScreen = (ScreenBase*)&printingScreen;
			activeScreen->displayControls(255);
			break;
		case SCREEN_PREHEAT:
			activeScreen->clearScreen();
			preheatScreen.init();
			activeScreen->freeMem();
			activeScreen = (ScreenBase*)&preheatScreen;
			activeScreen->displayControls(255);
			break;
		case SCREEN_STARTUP:
			activeScreen->clearScreen();
			startupScreen.init();
			activeScreen->freeMem();
			activeScreen = (ScreenBase*)&startupScreen;
			activeScreen->displayControls(255);
			break;
		case SCREEN_FILAMENT:
			activeScreen->clearScreen();
			activeScreen->freeMem();
			filamentScreen.init();
			activeScreen = (ScreenBase*)&filamentScreen;
			activeScreen->displayControls(255);
			break;
		case SCREEN_UTILITIES:
			activeScreen->clearScreen();
			activeScreen->freeMem();
			utilitiesScreen.init();
			activeScreen = (ScreenBase*)&utilitiesScreen;
			activeScreen->displayControls(255);
			break;
		case SCREEN_SETTINGS:
			activeScreen->clearScreen();
			activeScreen->freeMem();
			settingsScreen.init();
			activeScreen = (ScreenBase*)&settingsScreen;
			activeScreen->displayControls(255);
			break;
	}
}


#endif /* SCREENSMANAGER_H_ */