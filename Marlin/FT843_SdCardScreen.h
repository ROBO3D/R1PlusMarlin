/*
 * SdCardScreen.h
 *
 * Created: 21/10/2014 1:05:42 PM
 *  Author: Sherif
 */ 


#ifndef SDCARDSCREEN_H_
#define SDCARDSCREEN_H_

#include "FT843_ScreenBase.h"

#define SD_FILE_HEIGHT 40
#define SD_FILE_COUNT 5

#define CLR_SD_FILE_ONE COLOR_RGB(237, 237, 237)
#define CLR_SD_FILE_TWO CLR_WHITE

typedef struct SdCardScreenData
{
	uint32_t ptr_print_header;
	uint32_t ptr_down;
	uint32_t ptr_down_click;
	uint32_t ptr_up;
	uint32_t ptr_up_click;
	uint32_t ptr_back_home;
	uint32_t ptr_back_home_click;
	uint32_t ptr_right_print;
	uint32_t ptr_right_print_click;
	
	Bitmap_header_t* print_header;
	Bitmap_header_t* arrows_header;
	Bitmap_header_t* back_home_header;
	Bitmap_header_t* right_print_header;
	
	const int panels_number = 5;
	int16_t selectedFileID;
	uint16_t topFileID = 0;
	
	uint8_t systemVolumeIndex;
	int16_t diff;
	
	uint32_t ram_ptr;
	
}SdCardScreenData_t;

class SdCardScreen : public ScreenBase
{
	public:
		virtual void addControls(uint8_t alpha)
		{			
			this->tagPressed(0);
		}
		
		virtual void displayControls(uint8_t alpha)
		{
			data->ram_ptr = 0L;
			this->addHeader();
			
			FTObj.Cmd_DLStart();
			this->clear();
			
			writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 100, DEFAULT_FONT, PSTR("Initializing SD Card."));
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
			
			//card.initsd();
			loadImages();
			addControls(alpha);			
		}
	
		virtual void loadImages()
		{
			data->ptr_back_home = this->addImageInMemory(back_home_img, BACK_HOME_DATASIZE, BACK_HOME_INFLATED, data->back_home_header, &data->ram_ptr);
			data->ptr_back_home_click = this->addImageInMemory(back_home_click_img, BACK_HOME_CLICK_DATASIZE, BACK_HOME_INFLATED, data->back_home_header, &data->ram_ptr);
			
			data->ptr_up = this->addImageInMemory(up_img, UP_DATASIZE, UP_INFLATED, data->arrows_header, &data->ram_ptr);			
			data->ptr_up_click = this->addImageInMemory(up_click_img, UP_CLICK_DATASIZE, UP_INFLATED, data->arrows_header, &data->ram_ptr);
			
			data->ptr_down = this->addImageInMemory(down_img, DOWN_DATASIZE, DOWN_INFLATED, data->arrows_header, &data->ram_ptr);
			data->ptr_down_click = this->addImageInMemory(down_click_img, DOWN_CLICK_DATASIZE, DOWN_INFLATED, data->arrows_header, &data->ram_ptr);
			
			data->ptr_right_print = this->addImageInMemory(right_print_img, RIGHT_PRINT_DATASIZE, RIGHT_PRINT_INFLATED, data->right_print_header, &data->ram_ptr);
			data->ptr_right_print_click = this->addImageInMemory(right_print_click_img, RIGHT_PRINT_CLICK_DATASIZE, RIGHT_PRINT_INFLATED, data->right_print_header, &data->ram_ptr);
		}
		
		virtual void addHeader()
		{
			data->ptr_print_header = this->addImageInMemory(print_header_img, PRINT_HEADER_DATASIZE, PRINT_HEADER_INFLATED, data->print_header, &data->ram_ptr);			
			drawHeader();
		}

		virtual void drawHeader()
		{
			this->drawRectangle(0, 0, SCREEN_WIDTH, SPLIT_LINE_START - 1, HEADER_COLOR);
			int img_x = (SCREEN_WIDTH - data->print_header->Width) / 2;
			drawBitmap(data->ptr_print_header, data->print_header, img_x, 1, 255);
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
			
			if (card.cardOK)
			{
				drawFilesList(tag);
			}
			else
			{
				drawSdCardInit(tag);
			}
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
		}
		
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1:
					return SCREEN_HOME;
				case 2:
					data->topFileID = data->topFileID > 0 ? data->topFileID - 1 : data->topFileID;
					this->tagPressed(0);
					return 0;
				case 3:
					data->topFileID = data->topFileID < data->diff ? data->topFileID + 1 : data->topFileID;
					//SERIAL_ECHO_START;
					//SERIAL_ECHO("Difference: ");
					//SERIAL_ECHO(data->diff);
					//SERIAL_ECHO("\t TopFileID: ");
					//SERIAL_ECHOLN(data->topFileID);
					this->tagPressed(0);
					return 0;
				case 4:
					if (data->selectedFileID != -1)
					{
						data->selectedFileID += 1;
						card.getfilename(data->selectedFileID);
						card.openFile(card.filename, true);
						card.startFileprint();
						enquecommand_P(PSTR("M24"));
						starttime = millis();
						return SCREEN_PRINT_HEATING;
					}
					
					this->tagPressed(0);
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
					break;
				case 11:
					card.initsd();
					if (card.cardOK)
					{
						int16_t filesCount = card.getnrfilenames();
						data->diff = (filesCount - 1) - SD_FILE_COUNT;
					}
					
					this->tagPressed(0);
					break;
			}
			
			return 0;
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
		
		void init()
		{
			data = (SdCardScreenData_t*)malloc(sizeof(SdCardScreenData_t));
			data->print_header			= &Bitmap_headers[BH_PRINT_HEADER];
			data->arrows_header			= &Bitmap_headers[BH_ARROWS];
			data->back_home_header		= &Bitmap_headers[BH_BACK_HOME];
			data->right_print_header	= &Bitmap_headers[BH_RIGHT_PRINT];
			data->topFileID = 0;
			data->selectedFileID = -1;
			
			card.initsd();
			int16_t filesCount = card.getnrfilenames();
			data->diff = (filesCount - 1) - SD_FILE_COUNT;
		}
		
		
		SdCardScreenData_t* data;
	private:
		void drawFileNamePanels()
		{
			uint16_t filesCount = card.getnrfilenames();
			
			//SERIAL_ECHO_START;
			//SERIAL_ECHO("FilesCount: ");
			//SERIAL_ECHOLN(filesCount);
			
			uint32_t backColor = 0;
			uint32_t fontColor = CLR_WHITE;
			for (int i = 0; i < SD_FILE_COUNT; i++)
			{
				if ((i + 1) >= filesCount)
				{
					if ((i % 2) == 0)
					{
						drawRectangle(0, (SPLIT_LINE_START + 1 + (i * SD_FILE_HEIGHT)), 409, (SPLIT_LINE_START + ((i + 1) * SD_FILE_HEIGHT)), CLR_SD_FILE_ONE);
					}
					else
					{
						drawRectangle(0, (SPLIT_LINE_START + 1 + (i * SD_FILE_HEIGHT)), 409, (SPLIT_LINE_START + ((i + 1) * SD_FILE_HEIGHT)), CLR_SD_FILE_TWO);
					}
					
					continue;
				}
				
				card.getfilename((i + 1) + data->topFileID);
				if ((i + data->topFileID) == data->selectedFileID)
				{
					drawRectangle(0, (SPLIT_LINE_START + 1 + (i * SD_FILE_HEIGHT)), 409, (SPLIT_LINE_START + ((i + 1) * SD_FILE_HEIGHT)), CLR_PANEL_PRESSED, (5 + i));
					this->writeString(CLR_WHITE, 15, (SPLIT_LINE_START + 21 + (i * SD_FILE_HEIGHT)), DEFAULT_FONT, card.longFilename, FT_OPT_CENTERY, (5 + i));
				}
				else if ((i % 2) == 0)
				{
					drawRectangle(0, (SPLIT_LINE_START + 1 + (i * SD_FILE_HEIGHT)), 409, (SPLIT_LINE_START + ((i + 1) * SD_FILE_HEIGHT)), CLR_SD_FILE_ONE, (5 + i));
					this->writeString(CLR_BLACK, 15, (SPLIT_LINE_START + 21 + (i * SD_FILE_HEIGHT)), DEFAULT_FONT, card.longFilename, FT_OPT_CENTERY, (5 + i));
				}
				else
				{
					drawRectangle(0, (SPLIT_LINE_START + 1 + (i * SD_FILE_HEIGHT)), 409, (SPLIT_LINE_START + ((i + 1) * SD_FILE_HEIGHT)), CLR_SD_FILE_TWO, (5 + i));
					this->writeString(CLR_BLACK, 15, (SPLIT_LINE_START + 21 + (i * SD_FILE_HEIGHT)), DEFAULT_FONT, card.longFilename, FT_OPT_CENTERY, (5 + i));
				}
			}
		}
		
		void drawFilesList(uint8_t tag)
		{
			if (tag == 1)
				drawBitmap(data->ptr_back_home_click, data->back_home_header, 10, 1, 255, 1);
			else
				drawBitmap(data->ptr_back_home, data->back_home_header, 10, 1, 255, 1);
			
			this->drawRectangle(411, 80, SCREEN_WIDTH, 135, CLR_WHITE, 2);
			if (tag == 2)
				drawBitmap(data->ptr_up_click, data->arrows_header, 425, 95, 255, 2);
			else
				drawBitmap(data->ptr_up, data->arrows_header, 425, 95, 255, 2);
			
			this->drawRectangle(411, 220, SCREEN_WIDTH, SCREEN_HEIGHT, CLR_WHITE, 3);
			if (tag == 3)
				drawBitmap(data->ptr_down_click, data->arrows_header, 425, 220, 255, 3);
			else
				drawBitmap(data->ptr_down, data->arrows_header, 425, 220, 255, 3);
			
			if (tag == 4)
				drawBitmap(data->ptr_right_print_click, data->right_print_header, 380, 1, 255, 4);
			else
				drawBitmap(data->ptr_right_print, data->right_print_header, 380, 1, 255, 4);
			
			if (tag >= 5 && tag <= 9)
				data->selectedFileID = tag - 5 + data->topFileID;
			
			drawFileNamePanels();
			this->drawLine(410, 410, SPLIT_LINE_START, SCREEN_HEIGHT, CLR_SPLIT_LINE);
		}
		
		void drawSdCardInit(uint8_t tag)
		{
			writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 100, DEFAULT_FONT, PSTR("Unable to open SD card. Please connect it."));
			drawRectangle(100, 140, 380, 230, CLR_PANEL_PRESSED, 11);
			if (tag == 11)
			{
				writeString_p(CLR_SPLIT_LINE, 240, 185, 28, SDCARD_MOUNT_CARD, 11);
			}
			else
			{
				writeString_p(CLR_WHITE, 240, 185, 28, SDCARD_MOUNT_CARD, 11);
			}
		}
};




#endif /* SDCARDSCREEN_H_ */