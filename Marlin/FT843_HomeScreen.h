/*
 * HomeScreen.h
 *
 * Created: 16/10/2014 12:20:19 PM
 *  Author: Sherif
 */ 


#ifndef HOMESCREEN_H_
#define HOMESCREEN_H_

#include "FT843_ScreenBase.h"

typedef struct HomeData
{
	uint32_t ptr_print;
	uint32_t ptr_print_click;
	uint32_t ptr_preheat;
	uint32_t ptr_preheat_click;
	uint32_t ptr_filament;
	uint32_t ptr_filament_click;
	uint32_t ptr_utilities;
	uint32_t ptr_utilities_click;
	uint32_t ptr_settings;
	uint32_t ptr_settings_click;
	uint32_t header_ptr;
	
	Bitmap_header_t* screenHeader;
	Bitmap_header_t* print_header;
	Bitmap_header_t* preheat_header;
	Bitmap_header_t* filament_header;
	Bitmap_header_t* utilities_header;
	Bitmap_header_t* settings_header;
	
	int panels_number;
	uint32_t ram_ptr;
	
}HomeData_t;

class HomeScreen : public ScreenBase
{
	public:
		virtual void addControls(uint8_t alpha)
		{			
			this->tagPressed(0);
		}
		
		virtual void displayControls(uint8_t alpha)
		{
			data->ram_ptr = 0L;
			FTObj.Cmd_DLStart();
			this->clear();
			this->addHeader();
			
			loadImages();
			addControls(alpha);
			this->drawSplitLines(5);
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
			this->loadImagesSecondary();
		}
	
		virtual void loadImages()
		{
			// NORMAL
			data->ptr_print = this->addImageInMemory(print_img, PRINT_DATASIZE, PRINT_INFLATED, data->print_header, &data->ram_ptr);
			data->ptr_preheat = this->addImageInMemory(preheat_img, PREHEAT_DATASIZE, PREHEAT_INFLATED, data->preheat_header, &data->ram_ptr);
			data->ptr_filament = this->addImageInMemory(filament_img, FILAMENT_DATASIZE, FILAMENT_INFLATED, data->filament_header, &data->ram_ptr);
			data->ptr_utilities = this->addImageInMemory(utilities_img, UTILITIES_DATASIZE, UTILITIES_INFLATED, data->utilities_header, &data->ram_ptr);
			data->ptr_settings = this->addImageInMemory(settings_img, SETTINGS_DATASIZE, SETTINGS_INFLATED, data->settings_header, &data->ram_ptr);
		}
		
		void loadImagesSecondary()
		{
			// CLICK
			data->ptr_print_click = this->addImageInMemory(print_click_img, PRINT_CLICK_DATASIZE, PRINT_INFLATED, data->print_header, &data->ram_ptr);
			data->ptr_preheat_click = this->addImageInMemory(preheat_click_img, PREHEAT_CLICK_DATASIZE, PREHEAT_INFLATED, data->preheat_header, &data->ram_ptr);
			data->ptr_filament_click = this->addImageInMemory(filament_click_img, FILAMENT_CLICK_DATASIZE, FILAMENT_INFLATED, data->filament_header, &data->ram_ptr);
			data->ptr_utilities_click = this->addImageInMemory(utilities_click_img, UTILITIES_CLICK_DATASIZE, UTILITIES_INFLATED, data->utilities_header, &data->ram_ptr);
			data->ptr_settings_click = this->addImageInMemory(settings_click_img, SETTINGS_CLICK_DATASIZE, SETTINGS_INFLATED, data->settings_header, &data->ram_ptr);
		}
		
		virtual void tagPressed(uint8_t tag)
		{
			//SERIAL_ECHO_START;
			//SERIAL_ECHOLNPGM("Before cmd DL start");
			FTObj.Cmd_DLStart();
			this->clear();
			this->drawHeader();
			
			if (tag == 1)
				drawPanelBitmap(data->ptr_print_click, data->print_header, 0, CLR_PANEL_PRESSED);
			else
				drawPanelBitmap(data->ptr_print, data->print_header, 0, CLR_WHITE);
				
			if (tag == 2)
				drawPanelBitmap(data->ptr_preheat_click, data->preheat_header, 1, CLR_PANEL_PRESSED);
			else
				drawPanelBitmap(data->ptr_preheat, data->preheat_header, 1, CLR_WHITE);
			
			if (tag == 3)
				drawPanelBitmap(data->ptr_filament_click, data->filament_header, 2, CLR_PANEL_PRESSED);
			else
				drawPanelBitmap(data->ptr_filament, data->filament_header, 2, CLR_WHITE);
			
			if (tag == 4)
				drawPanelBitmap(data->ptr_utilities_click, data->utilities_header, 3, CLR_PANEL_PRESSED);
			else
				drawPanelBitmap(data->ptr_utilities, data->utilities_header, 3, CLR_WHITE);
				
			if (tag == 5)
				drawPanelBitmap(data->ptr_settings_click, data->settings_header, 4, CLR_PANEL_PRESSED);
			else
				drawPanelBitmap(data->ptr_settings, data->settings_header, 4, CLR_WHITE);
			
			this->drawSplitLines(5);
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
			
			//SERIAL_ECHO_START;
			//SERIAL_ECHOLNPGM("After swap");
		}
		
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1: // Print
					return SCREEN_PRINT_SDCARD;
				case 2: // Preheat
					return SCREEN_PREHEAT;
				case 3: // Filament
					return SCREEN_FILAMENT;
				case 4: // Utilities
					return SCREEN_UTILITIES;
				case 5: // Settings
					return SCREEN_SETTINGS;
			}
			
		}
		
		virtual void addHeader()
		{
			data->header_ptr = this->addImageInMemory(home_header, HOME_HEADER_DATASIZE, HOME_HEADER_INFLATED, data->screenHeader, &data->ram_ptr);
			drawHeader();
		}

		virtual void drawHeader()
		{
			this->drawRectangle(0, 0, SCREEN_WIDTH, SPLIT_LINE_START - 1, HEADER_COLOR);
			int img_x = (SCREEN_WIDTH - data->screenHeader->Width) / 2;
			
			//#if ROBO_VERSION == ROBO_R1
			//this->writeString_p(CLR_WHITE, SCREEN_WIDTH / 2, 36, 30, PSTR("R1"));
			//#else
			drawBitmap(data->header_ptr, data->screenHeader, img_x, 1, 255);
			//#endif
		}
		
		void init()
		{
			SERIAL_ECHO_START;
			SERIAL_ECHOLNPGM("Home screen");
			data = (HomeData_t*)malloc(sizeof(HomeData_t));
			data->screenHeader			= &Bitmap_headers[BH_HOME_HEADER];
			data->print_header			= &Bitmap_headers[BH_HOME_SCREEN];
			data->preheat_header		= &Bitmap_headers[BH_HOME_SCREEN];
			data->filament_header		= &Bitmap_headers[BH_HOME_SCREEN];
			data->utilities_header		= &Bitmap_headers[BH_HOME_SCREEN];
			data->settings_header		= &Bitmap_headers[BH_HOME_SCREEN];
			
			data->panels_number = 5;
			data->ram_ptr = 0L;
		}
		
		virtual void freeMem()
		{
			free(data);
		}
		
		virtual uint8_t process()
		{
			this->tagPressed(0);
			return 0;
		}
		
	private:
		void drawPanelBitmap(uint32_t ramLocation, Bitmap_header_t* header, int panelID, uint32_t clr)
		{
			int panelWidth = (SCREEN_WIDTH - (data->panels_number - 1)) / data->panels_number;
			
			// Calculating height of bitmap inside panel to be centered
			int y = ((SCREEN_HEIGHT - SPLIT_LINE_START) - header->Height) / 2;
			y += SPLIT_LINE_START;
			
			// Drawing the rectangle behind the image
			int tag = panelID + 1;
			int x0 = (panelWidth * panelID) + (panelID + 1);
			x0 = panelID == 0 ? 0 : x0;
			int x1 = (x0 + panelWidth);
			this->drawRectangle(x0, SPLIT_LINE_START, x1, SCREEN_HEIGHT, clr, tag);
			
			int img_x = (panelWidth - header->Width) / 2;
			img_x += (panelWidth * panelID) + (panelID + 1);
			this->drawBitmap(ramLocation, header, img_x, y, 255, tag);
		}
		
		HomeData_t* data;
};

#endif /* HOMESCREEN_H_ */
