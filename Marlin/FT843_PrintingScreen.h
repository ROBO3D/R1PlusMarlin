/*
 * PrintingScreen.h
 *
 * Created: 23/10/2014 7:09:15 PM
 *  Author: Sherif
 */ 


#ifndef PRINTINGSCREEN_H_
#define PRINTINGSCREEN_H_


#include "FT843_ScreenBase.h"

#define PRINTING_MODE_PRINTING	0
#define PRINTING_MODE_PAUSED	1
#define PRINTING_MODE_CANCELED 2

#define CLR_PRINT_PROGRESS COLOR_RGB(202, 202, 202)

typedef struct PrintingScreenData
{
	uint8_t hour = 1;
	uint8_t minute = 33;
	uint8_t second = 37;
	
	uint8_t percent = 0;
	uint8_t printFinished = 0;
	
	uint32_t totalTime = 0;
	
	uint32_t ptr_printing_header;
	uint32_t ptr_printing_paused_header;
	uint32_t ptr_print_canceled_header;
	
	Bitmap_header_t* printing_header;
	Bitmap_header_t* printing_paused_header;
	Bitmap_header_t* print_canceled_header;
	
	uint8_t mode;
	uint32_t ram_ptr;
	
}PrintingScreenData_t;

class PrintingScreen : public ScreenBase
{
	public:
		virtual void addControls(uint8_t alpha)
		{
			this->tagPressed(0);
		}
	
		virtual void displayControls(uint8_t alpha)
		{
			data->ram_ptr = 0L;
			
			loadImages();
			this->addHeader();
		
			addControls(alpha);
		}
	
		virtual void loadImages()
		{
			data->ptr_printing_header = this->addImageInMemory(printing_header_img, PRINTING_HEADER_DATASIZE, PRINTING_HEADER_INFLATED, data->printing_header, &data->ram_ptr);
			
			data->ptr_printing_paused_header = this->addImageInMemory(printing_paused_header_img, PRINTING_PAUSED_HEADER_DATASIZE, PRINTING_PAUSED_HEADER_INFLATED, data->printing_paused_header, &data->ram_ptr);
			
			data->ptr_print_canceled_header = this->addImageInMemory(print_canceled_img, PRINT_CANCELED_HEADER_DATASIZE, PRINT_CANCELED_HEADER_INFLATED, data->print_canceled_header, &data->ram_ptr);
		}
	
		virtual void addHeader()
		{
			drawHeader();
		}

		virtual void drawHeader()
		{
			this->drawRectangle(0, 0, SCREEN_WIDTH, SPLIT_LINE_START - 1, HEADER_COLOR);
			int img_x;
			switch (data->mode)
			{
				case PRINTING_MODE_PRINTING:
					img_x = (SCREEN_WIDTH - data->printing_header->Width) / 2;
					drawBitmap(data->ptr_printing_header, data->printing_header, img_x, 1, 255);
					break;
				case PRINTING_MODE_PAUSED:
					img_x = (SCREEN_WIDTH - data->printing_paused_header->Width) / 2;
					drawBitmap(data->ptr_printing_paused_header, data->printing_paused_header, img_x, 1, 255);
					break;
				case PRINTING_MODE_CANCELED:
					img_x = (SCREEN_WIDTH - data->print_canceled_header->Width) / 2;
					drawBitmap(data->ptr_print_canceled_header, data->print_canceled_header, img_x, 1, 255);
					break;
			}
		}
	
		virtual void tagPressed(uint8_t tag)
		{
			FTObj.Cmd_DLStart();
			this->clear();
			this->drawHeader();
			
			switch (data->mode)
			{
				case PRINTING_MODE_PRINTING:
					this->tagPressedPrinting(tag);
					break;
				case PRINTING_MODE_PAUSED:
					this->tagPressedPaused(tag);
					break;
				case PRINTING_MODE_CANCELED:
					this->tagPressedCanceled(tag);
					break;
			}		
		
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
		}
		
		virtual uint8_t process()
		{
			if (!data->percent != 100)
			{
				data->percent = card.percentDone();
			}
			
			//if (data->percent == 100)
			//{
				//data->printFinished = 1;
			//}
			
			this->tagPressed(0);
			return 0;
		}
		
		void tagPressedPrinting(uint8_t tag)
		{
			//card.getfilename(data->selectedFileID);
			
			
			if (!card.sdprinting)
			{	
				if (!data->printFinished)
				{
					data->printFinished = 1;
					data->totalTime = millis() - starttime;
				}
				
				this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 94, DEFAULT_FONT, PSTR("Printing Complete"));
				this->drawRectangle(120, 205, 360, 270, CLR_WHITE, 3);
				this->drawRectangle(138, 215, 348, 255, CLR_PANEL_PRESSED, 3);
				if (tag == 3)
					this->writeString_p(CLR_SPLIT_LINE, 183, 235, DEFAULT_FONT, PRINTING_RETURNHOME, FT_OPT_CENTERY, 3);
				else
					this->writeString_p(CLR_WHITE, 183, 235, DEFAULT_FONT, PRINTING_RETURNHOME, FT_OPT_CENTERY, 3);
			}
			else
			{
				char* res = this->concat("Now printing ", card.longFilename);
				this->writeString(CLR_BLACK, SCREEN_WIDTH / 2, 94, DEFAULT_FONT, res);
				free(res);
				
				this->writeTimeLeft(122);
				this->writePercentage(152);
				
				
				this->drawRectangle(20, 176, 460, 196, CLR_PRINT_PROGRESS);
				this->drawRectangle(20, 176, getWidthFromPerc(), 196, CLR_PANEL_PRESSED);
				
				this->drawRectangle(20, 215, 230, 255, CLR_PANEL_PRESSED, 1);
				if (tag == 1)
					this->writeString_p(CLR_SPLIT_LINE, 72, 235, DEFAULT_FONT, PRINTING_PAUSE, FT_OPT_CENTERY, 1);
				else	
					this->writeString_p(CLR_WHITE, 72, 235, DEFAULT_FONT, PRINTING_PAUSE, FT_OPT_CENTERY, 1);
				
				this->drawRectangle(252, 215, 462, 255, CLR_PANEL_PRESSED, 2);
				if (tag == 2)
					this->writeString_p(CLR_SPLIT_LINE, 309, 235, DEFAULT_FONT, PRINTING_ABORT, FT_OPT_CENTERY, 2);
				else
					this->writeString_p(CLR_WHITE, 309, 235, DEFAULT_FONT, PRINTING_ABORT, FT_OPT_CENTERY, 2);
			}
		}
		
		void tagPressedPaused(uint8_t tag)
		{
			//card.getfilename(data->selectedFileID);
			char* res = this->concat(card.longFilename, " - paused");
			this->writeString(CLR_BLACK, SCREEN_WIDTH / 2, 94, DEFAULT_FONT, res);
			free(res);
			
			this->drawRectangle(137, 122, 347, 162, CLR_PANEL_PRESSED, 4);
			if (tag == 4)
				this->writeString_p(CLR_SPLIT_LINE, 185, 140, DEFAULT_FONT, PRINTING_RESUME, FT_OPT_CENTERY, 4);
			else
				this->writeString_p(CLR_WHITE, 185, 140, DEFAULT_FONT, PRINTING_RESUME, FT_OPT_CENTERY, 4);
			
			this->writeTimeLeft(185);
			this->writePercentage(214);
			
			this->drawRectangle(20, 235, 460, 255, CLR_PRINT_PROGRESS);
			this->drawRectangle(20, 235, getWidthFromPerc(), 255, CLR_PANEL_PRESSED);
		}
		
		void tagPressedCanceled(uint8_t tag)
		{
			//card.getfilename(data->selectedFileID);
			char* res = this->concat(card.longFilename, " - canceled");
			this->writeString(CLR_BLACK, SCREEN_WIDTH / 2, 94, DEFAULT_FONT, res);
			free(res);
			
			this->drawRectangle(120, 205, 360, 270, CLR_WHITE, 3);
			this->drawRectangle(137, 122, 347, 162, CLR_PANEL_PRESSED, 3);
			if (tag == 3)
				this->writeString_p(CLR_SPLIT_LINE, 185, 140, DEFAULT_FONT, PRINTING_RETURNHOME, FT_OPT_CENTERY, 3);
			else
				this->writeString_p(CLR_WHITE, 185, 140, DEFAULT_FONT, PRINTING_RETURNHOME, FT_OPT_CENTERY, 3);
			
			this->writePercentage(214);
			this->drawRectangle(20, 235, 460, 255, CLR_PRINT_PROGRESS);
			this->drawRectangle(20, 235, getWidthFromPerc(), 255, CLR_PANEL_PRESSED);
		}
	
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1:
					data->mode = PRINTING_MODE_PAUSED;
					card.pauseSDPrint();
					tagPressed(0);
					return 0;
				case 2:
					data->mode = PRINTING_MODE_CANCELED;
					
					card.sdprinting = false;
					card.closefile();
					quickStop();
					if(SD_FINISHED_STEPPERRELEASE)
					{
						enquecommand_P(PSTR(SD_FINISHED_RELEASECOMMAND));
					}
					autotempShutdown();
					clearTemp();
					starttime = 0;
					
					tagPressed(0);
					return 0;
				case 3:
					return SCREEN_HOME;
				case 4:
					card.startFileprint();
					starttime = millis();
					data->mode = PRINTING_MODE_PRINTING;
					tagPressed(0);
					return 0;
				case 5:
				case 6:
				case 7:
				case 8:
					return 0;
			}
		}
	
		virtual void freeMem()
		{
			free(data);
		}
		
		void init()
		{
			SERIAL_ECHO_START;
			SERIAL_ECHOLNPGM("Printing screen");
			
			data = (PrintingScreenData_t*)malloc(sizeof(PrintingScreenData_t));
			data->hour = 1;
			data->minute = 33;
			data->second = 37;
			
			data->percent = 0;
			
			data->printing_header				= &Bitmap_headers[BH_PRINTING_HEADER];
			data->printing_paused_header		= &Bitmap_headers[BH_PRINTING_PAUSED];
			data->print_canceled_header			= &Bitmap_headers[BH_PRINT_CANCELED];
			
			data->mode = PRINTING_MODE_PRINTING;
		}
		
		
		PrintingScreenData_t* data;
	private:
		
		char* concat(const char* s1, const char* s2)
		{			
			char* res = (char*)malloc(strlen(s1) + 14);
			strcpy_P(res, PSTR("Now printing "));
			strcat(res, s2);
			return res;
		}
		
		void writeTimeLeft(uint16_t y)
		{
			long timeSeconds = (millis() / 1000L) - (starttime / 1000L);
			
			data->hour = timeSeconds / 3600;
			data->minute = (timeSeconds / 60) % 60;
			data->second = timeSeconds % 60;
			
			char* res = (char*)malloc(25);
			sprintf_P(res, PSTR("Time Elapsed: "));
			if (data->hour < 10)
			{
				sprintf_P((res + 14), PSTR("0%d:"), data->hour);
			}
			else
			{
				sprintf_P((res + 14), PSTR("%d:"), data->hour);
			}
			
			if (data->minute < 10)
			{
				sprintf_P((res + 17), PSTR("0%d:"), data->minute);
			}
			else
			{
				sprintf_P((res + 17), PSTR("%d:"), data->minute);
			}
			
			if (data->second < 10)
			{
				sprintf_P((res + 20), PSTR("0%d"), data->second);
			}
			else
			{
				sprintf_P((res + 20), PSTR("%d"), data->second);
			}
			
			this->writeString(CLR_BLACK, SCREEN_WIDTH / 2, y, 23, res);
			free(res);
		}
		
		void writePercentage(uint16_t y)
		{
			char* res = (char*)malloc(25);
			sprintf_P(res, PSTR("Percent complete: %d%%"), data->percent);
			this->writeString(CLR_BLACK, SCREEN_WIDTH / 2, y, 23, res);
			free(res);
		}
		
		uint16_t getWidthFromPerc()
		{
			float percenetage = (float)data->percent / 100.0F;
			return ((uint16_t)(percenetage * 440.0F) + 20);
		}
		
		void clearTemp()
		{
			setTargetHotend(0, 0);
			setTargetHotend(0, 1);
			setTargetBed(0);
		}
};


#endif /* PRINTINGSCREEN_H_ */