/*
 * FT843_SettingsScreen.h
 *
 * Created: 2/12/2014 6:12:56 PM
 *  Author: Sherif
 */ 


#ifndef FT843_SETTINGSSCREEN_H_
#define FT843_SETTINGSSCREEN_H_

#include "FT843_ScreenBase.h"

#define SETTINGS_SCREEN_START		0
#define SETTINGS_SCREEN_STATISTICS	1
#define SETTINGS_SCREEN_NETWORK		2

typedef struct SettingsScreenData
{
	Bitmap_header_t* back_home_header;
	
	uint32_t ptr_back_home;
	uint32_t ptr_back_home_click;
	
	uint8_t screen;
	
	uint32_t ram_ptr;
	
}SettingsScreenData_t;

class SettingsScreen : public ScreenBase
{
	public:
		virtual void addControls(uint8_t alpha)
		{
			this->tagPressed(0);
		}
		
		virtual void displayControls(uint8_t alpha)
		{
			loadImages();
			addHeader();
			addControls(alpha);
		}
		
		virtual void loadImages()
		{
			data->ptr_back_home = this->addImageInMemory(back_home_img, BACK_HOME_DATASIZE, BACK_HOME_INFLATED, data->back_home_header, &data->ram_ptr);
			data->ptr_back_home_click = this->addImageInMemory(back_home_click_img, BACK_HOME_CLICK_DATASIZE, BACK_HOME_INFLATED, data->back_home_header, &data->ram_ptr);
		}
		
		virtual void tagPressed(uint8_t tag)
		{
			FTObj.Cmd_DLStart();
			this->clear();
			
			this->drawHeader();
			
			if (tag == 1)
				drawBitmap(data->ptr_back_home_click, data->back_home_header, 10, 1, 255, 1);
			else
				drawBitmap(data->ptr_back_home, data->back_home_header, 10, 1, 255, 1);
			
		
			if (data->screen == SETTINGS_SCREEN_START)
			{
				drawStartScreen(tag);
			}
			else if (data->screen == SETTINGS_SCREEN_STATISTICS)
			{
				drawStatisticsScreen(tag);
			}
			else if (data->screen == SETTINGS_SCREEN_NETWORK)
			{
				drawNetworkSettingsScreen(tag);
			}			
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
		}
		
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1: // Return Home
					return SCREEN_HOME;
				case 2: // Level Platform
					data->screen = SETTINGS_SCREEN_NETWORK;
					this->tagPressed(0);
					break;
				case 3: // System Tools
					data->screen = SETTINGS_SCREEN_STATISTICS;
					this->tagPressed(0);
					break;
			}
			
			return 0;
		}
		
		virtual void addHeader()
		{
			
		}
		
		virtual void drawHeader()
		{
			this->drawRectangle(0, 0, SCREEN_WIDTH, SPLIT_LINE_START - 1, HEADER_COLOR);
			this->writeString_p(CLR_WHITE, SCREEN_WIDTH / 2, 36, 30, PSTR("SETTINGS"));
		}
		
		virtual uint8_t process()
		{			
			//this->tagPressed(0);
			return 0;
		}
		
		void init()
		{
			data = (SettingsScreenData_t*)malloc(sizeof(SettingsScreenData_t));
			data->ram_ptr = 0L;
			
			data->back_home_header = &Bitmap_headers[BH_BACK_HOME];
			data->screen = SETTINGS_SCREEN_START;
		}
		
		virtual void freeMem()
		{
			free(data);
		}
		
		SettingsScreenData_t* data;
	private:
	
		void drawStartScreen(uint8_t tag)
		{
			this->drawRectangle(20, 100, 230, 242, HEADER_COLOR, 2);
			if (tag == 2)
			{
				this->writeString_p(CLR_SPLIT_LINE, 125, 150, 30, SETTINGS_NETWORK, 2);
				this->writeString_p(CLR_SPLIT_LINE, 125, 190, 30, SETTINGS_SETTINGS, 2);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 125, 150, 30, SETTINGS_NETWORK, 2);
				this->writeString_p(CLR_WHITE, 125, 190, 30, SETTINGS_SETTINGS, 2);
			}
			
			this->drawRectangle(250, 100, 460, 242, HEADER_COLOR, 3);
			if (tag == 3)
			{
				this->writeString_p(CLR_SPLIT_LINE, 355, 170, 30, SETTINGS_STATISTICS, 3);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 355, 170, 30, SETTINGS_STATISTICS, 3);
			}
		}
		
		void drawStatisticsScreen(uint8_t tag)
		{
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, ((SCREEN_HEIGHT - HEADER_HEIGHT) / 2), 30, PSTR("Coming Soon"));
		}
		
		void drawNetworkSettingsScreen(uint8_t tag)
		{
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, ((SCREEN_HEIGHT - HEADER_HEIGHT) / 2), 30, PSTR("Coming Soon"));
		}
};


#endif /* FT843_SETTINGSSCREEN_H_ */