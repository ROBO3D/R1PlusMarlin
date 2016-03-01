/*
 * FT843_Utilities.h
 *
 * Created: 1/12/2014 8:17:35 PM
 *  Author: Sherif
 */ 


#ifndef FT843_UTILITIESSCREEN_H_
#define FT843_UTILITIESSCREEN_H_

#include "FT843_ScreenBase.h"

#define UTILITIES_SCREEN_DEFAULT 0
#define UTILITIES_SCREEN_SYSTEMTOOLS 1

typedef struct UtilitiesScreenData
{
	uint8_t screen;
	
	Bitmap_header_t* back_home_header;
	
	uint32_t ptr_back_home;
	uint32_t ptr_back_home_click;
	
	uint32_t ram_ptr;
	
}UtilitiesScreenData_t;

class UtilitiesScreen : public ScreenBase
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
			
		
			if (data->screen == UTILITIES_SCREEN_DEFAULT)
			{
				drawDefaultScreen(tag);
			}
			else if (data->screen = UTILITIES_SCREEN_SYSTEMTOOLS)
			{
				drawSystemToolsScreen(tag);
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
					return SCREEN_STARTUP;
					break;
				case 3: // System Tools
					data->screen = UTILITIES_SCREEN_SYSTEMTOOLS;
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
			this->writeString_p(CLR_WHITE, SCREEN_WIDTH / 2, 36, 30, PSTR("UTILITIES"));
		}
		
		virtual uint8_t process()
		{			
			//this->tagPressed(0);
			return 0;
		}
		
		void init()
		{
			data = (UtilitiesScreenData_t*)malloc(sizeof(UtilitiesScreenData_t));
			data->ram_ptr = 0L;
			
			data->back_home_header = &Bitmap_headers[BH_BACK_HOME];
			
			data->screen = UTILITIES_SCREEN_DEFAULT;
		}
		
		virtual void freeMem()
		{
			free(data);
		}
		
		UtilitiesScreenData_t* data;
	private:
		
		#if ROBO_VERSION == ROBO_R2 || ROBO_VERSION == ROBO_MINI || ROBO_VERSION == ROBO_R2_RAMPS
		void drawDefaultScreen(uint8_t tag)
		{
			this->drawRectangle(20, 100, 230, 242, HEADER_COLOR, 2);
			if (tag == 2)
			{
				this->writeString_p(CLR_SPLIT_LINE, 125, 150, 30, UTIL_LEVEL, 2);
				this->writeString_p(CLR_SPLIT_LINE, 125, 190, 30, UTIL_PLATFORM, 2);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 125, 150, 30, UTIL_LEVEL, 2);
				this->writeString_p(CLR_WHITE, 125, 190, 30, UTIL_PLATFORM, 2);
			}
			
			this->drawRectangle(250, 100, 460, 242, HEADER_COLOR, 3);
			if (tag == 3)
			{
				this->writeString_p(CLR_SPLIT_LINE, 355, 150, 30, UTIL_SYSTEM, 3);
				this->writeString_p(CLR_SPLIT_LINE, 355, 190, 30, UTIL_TOOLS, 3);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 355, 150, 30, UTIL_SYSTEM, 3);
				this->writeString_p(CLR_WHITE, 355, 190, 30, UTIL_TOOLS, 3);
			}
		}
		#endif
		
		#if ROBO_VERSION == ROBO_R1 || ROBO_VERSION == ROBO_R1_PLUS || ROBO_VERSION == ROBO_MEGA
		void drawDefaultScreen(uint8_t tag)
		{
			this->drawRectangle(135, 100, 345, 242, HEADER_COLOR, 3);
			if (tag == 3)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 150, 30, UTIL_SYSTEM, 3);
				this->writeString_p(CLR_SPLIT_LINE, 240, 190, 30, UTIL_TOOLS, 3);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 150, 30, UTIL_SYSTEM, 3);
				this->writeString_p(CLR_WHITE, 240, 190, 30, UTIL_TOOLS, 3);
			}
		}
		#endif
		
		void drawSystemToolsScreen(uint8_t tag)
		{
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, ((SCREEN_HEIGHT - HEADER_HEIGHT) / 2), 30, PSTR("Coming Soon"));
		}
};


#endif /* FT843_UTILITIESSCREEN_H_ */