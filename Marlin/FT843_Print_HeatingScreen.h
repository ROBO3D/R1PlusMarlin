/*
 * Print_Heating.h
 *
 * Created: 22/10/2014 10:04:56 PM
 *  Author: Sherif
 */ 


#ifndef PRINT_HEATINGSCREEN_H_
#define PRINT_HEATINGSCREEN_H_

#define HEAT_BOX_HEIGHT 60
#define HEAT_BOX_WIDTH 90
#define HEAT_BOX_SPACE 45

#define HEAT_BOX_Y 125
#define HEAT_BOX1_X 60
#define HEAT_BOX2_X (60 + HEAT_BOX_WIDTH + HEAT_BOX_SPACE)
#define HEAT_BOX3_X (60 + (HEAT_BOX_WIDTH * 2) + (HEAT_BOX_SPACE * 2))

#define CLR_HEATBOX COLOR_RGB(202, 202, 202)

#include "FT843_ScreenBase.h"

typedef struct PrintHeatingData
{
	uint32_t ptr_print_header;
	uint32_t ptr_back_sdcard;
	uint32_t ptr_back_sdcard_click;
	
	uint32_t timeBeforeHeating;
	
	Bitmap_header_t* print_header;
	Bitmap_header_t* back_sdcard_header;
	
	uint8_t nozel1 = 0;
	uint8_t nozel2 = 0;
	uint8_t bed = 0;
	uint8_t readyFlag = 0;
	
	uint32_t printStartTime;
	
	uint32_t ram_ptr;
	
}PrintHeatingData_t;

class PrintHeatingScreen : public ScreenBase
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
		
			loadImages();
			addControls(alpha);
		}
	
		virtual void loadImages()
		{
			data->ptr_back_sdcard = this->addImageInMemory(back_sdcard_img, BACK_SDCARD_DATASIZE, BACK_SDCARD_INFLATED, data->back_sdcard_header, &data->ram_ptr);
			data->ptr_back_sdcard_click = this->addImageInMemory(back_sdcard_click_img, BACK_SDCARD_CLICK_DATASIZE, BACK_SDCARD_INFLATED, data->back_sdcard_header, &data->ram_ptr);
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
				drawBitmap(data->ptr_back_sdcard_click, data->back_sdcard_header, 10, 1, 255, 1);
			else
				drawBitmap(data->ptr_back_sdcard, data->back_sdcard_header, 10, 1, 255, 1);
		
			//card.getfilename(data->selectedFileID);
			this->writeString(CLR_BLACK, SCREEN_WIDTH / 2, 94, DEFAULT_FONT, card.longFilename);
			
			#if ROBO_VERSION == ROBO_R1 || ROBO_VERSION == ROBO_R1_PLUS
			
			#elif ROBO_VERSION == ROBO_R2 || ROBO_VERSION == ROBO_MEGA
			
			#elif ROBO_VERSION == ROBO_MINI
			
			#endif
			drawHeatBox();
		}
	
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1:
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
					
					return SCREEN_PRINT_SDCARD;
			}
			
			return 0;
		}
	
		virtual void freeMem()
		{
			free(data);
		}
		
		virtual uint8_t process()
		{			
			//if (((millis() - starttime) % 3500) >= 3000)
			//{
				//enquecommand_P(PSTR("M27"));
			//}
			
			//if (((millis() - starttime) % 3500) >= 3000)
			//{
				//enquecommand_P(PSTR("M27"));
			//}
			
			//SERIAL_ECHO_START;
			//SERIAL_ECHO("Bed degree: ");
			//SERIAL_ECHO(degBed());
			//SERIAL_ECHO("Target: ");
			//SERIAL_ECHOLN(degTargetBed());
			
			this->tagPressed(0);
			
			if (data->readyFlag == 1 || isHeatingComplete())
			{
				clearHeatingFlag();
				return SCREEN_PRINTING;
			}
			
			return 0;
		}
	
		void init()
		{
			SERIAL_ECHO_START;
			SERIAL_ECHOLNPGM("PrintHeating screen");
			data = (PrintHeatingData_t*)malloc(sizeof(PrintHeatingData_t));
			data->nozel1 = 0;
			data->nozel2 = 0;
			data->bed = 0;
			data->readyFlag = 0;
			
			data->print_header				= &Bitmap_headers[BH_PRINT_HEADER];
			data->back_sdcard_header		= &Bitmap_headers[BH_BACK_SDCARD];
			
			data->timeBeforeHeating = millis();
			data->printStartTime = starttime;
		}
		
		PrintHeatingData_t* data;
		
	private:
		
		uint16_t getPercentageHeight(uint8_t perc)
		{
			perc = perc > 100 ? 100 : perc;
			float p = (float)perc / 100.0F;
			p = p * HEAT_BOX_HEIGHT;
			p = (HEAT_BOX_Y + HEAT_BOX_HEIGHT) - p;
			return (uint16_t)p;
		}
		
		#if ROBO_VERSION == ROBO_R2 || ROBO_VERSION == ROBO_MEGA || ROBO_VERSION == ROBO_R2_RAMPS
		void drawHeatBox()
		{
			this->drawRectangle(HEAT_BOX1_X, HEAT_BOX_Y, (HEAT_BOX1_X + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_HEATBOX);
			this->drawRectangle(HEAT_BOX1_X, getPercentageHeight(data->nozel1), (HEAT_BOX1_X + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_PANEL_PRESSED);
			this->writeString_p(CLR_BLACK, (HEAT_BOX1_X + (HEAT_BOX_WIDTH / 2)), 201, HEAT_BOX_FONT, PRINT_HEATING_NOZEL1TEMP);
			if (data->nozel1 >= 100)
				this->writeString_p(CLR_WHITE, (HEAT_BOX1_X + (HEAT_BOX_WIDTH / 2)), (HEAT_BOX_Y + (HEAT_BOX_HEIGHT / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			
			this->drawRectangle(HEAT_BOX2_X, HEAT_BOX_Y, (HEAT_BOX2_X + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_HEATBOX);
			this->drawRectangle(HEAT_BOX2_X, getPercentageHeight(data->nozel2), (HEAT_BOX2_X + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_PANEL_PRESSED);
			this->writeString_p(CLR_BLACK, (HEAT_BOX2_X + (HEAT_BOX_WIDTH / 2)), 201, HEAT_BOX_FONT, PRINT_HEATING_NOZEL2TEMP);
			if (data->nozel2 >= 100)
				this->writeString_p(CLR_WHITE, (HEAT_BOX2_X + (HEAT_BOX_WIDTH / 2)), (HEAT_BOX_Y + (HEAT_BOX_HEIGHT / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			
			
			this->drawRectangle(HEAT_BOX3_X, HEAT_BOX_Y, (HEAT_BOX3_X + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_HEATBOX);
			this->drawRectangle(HEAT_BOX3_X, getPercentageHeight(data->bed), (HEAT_BOX3_X + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_PANEL_PRESSED);
			this->writeString_p(CLR_BLACK, (HEAT_BOX3_X + (HEAT_BOX_WIDTH / 2)), 201, HEAT_BOX_FONT, PRINT_HEATING_BEDTEMP);
			if (data->bed >= 100)
				this->writeString_p(CLR_WHITE, (HEAT_BOX3_X + (HEAT_BOX_WIDTH / 2)), (HEAT_BOX_Y + (HEAT_BOX_HEIGHT / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			
			if (data->nozel1 == 100 && data->nozel2 == 100 && data->bed == 100)
			{
				this->drawRectangle(20, 220, 460, 260, CLR_PANEL_PRESSED);
				this->writeString_p(CLR_WHITE, 245, 240, DEFAULT_FONT, PRINT_HEATING_READY);
				FTObj.DLEnd();
				FTObj.Cmd_Swap();
				data->readyFlag = 1;
				//delay(500);
			}
			else
			{
				this->drawRectangle(20, 220, 460, 260, CLR_HEATBOX);
				this->writeString_p(CLR_WHITE, 245, 240, DEFAULT_FONT, PRINT_HEATING_PREPARE);
				FTObj.DLEnd();
				FTObj.Cmd_Swap();
			}
			
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
			data->nozel2 = (uint8_t)((degHotend(1) / degTargetHotend(1)) * 100.0F);
			data->bed	 = (uint8_t)((degBed() / degTargetBed()) * 100.0F);
		}
		#endif
		
		#if ROBO_VERSION == ROBO_R1 || ROBO_VERSION == ROBO_R1_PLUS
		void drawHeatBox()
		{
			this->drawRectangle((HEAT_BOX1_X + 30), HEAT_BOX_Y, ((HEAT_BOX1_X + 30) + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_HEATBOX);
			this->drawRectangle((HEAT_BOX1_X + 30), getPercentageHeight(data->nozel1), ((HEAT_BOX1_X + 30) + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_PANEL_PRESSED);
			this->writeString_p(CLR_BLACK, ((HEAT_BOX1_X + 30) + (HEAT_BOX_WIDTH / 2)), 201, HEAT_BOX_FONT, PRINT_HEATING_NOZEL1TEMP);
			if (data->nozel1 >= 100)
				this->writeString_p(CLR_WHITE, ((HEAT_BOX1_X + 30) + (HEAT_BOX_WIDTH / 2)), (HEAT_BOX_Y + (HEAT_BOX_HEIGHT / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			
			this->drawRectangle((HEAT_BOX3_X - 30), HEAT_BOX_Y, ((HEAT_BOX3_X - 30) + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_HEATBOX);
			this->drawRectangle((HEAT_BOX3_X - 30), getPercentageHeight(data->bed), ((HEAT_BOX3_X - 30) + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_PANEL_PRESSED);
			this->writeString_p(CLR_BLACK, ((HEAT_BOX3_X - 30) + (HEAT_BOX_WIDTH / 2)), 201, HEAT_BOX_FONT, PRINT_HEATING_BEDTEMP);
			if (data->bed >= 100)
				this->writeString_p(CLR_WHITE, ((HEAT_BOX3_X - 30) + (HEAT_BOX_WIDTH / 2)), (HEAT_BOX_Y + (HEAT_BOX_HEIGHT / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			
			if (data->nozel1 == 100 && data->bed == 100)
			{
				this->drawRectangle(20, 220, 460, 260, CLR_PANEL_PRESSED);
				this->writeString_p(CLR_WHITE, 245, 240, DEFAULT_FONT, PRINT_HEATING_READY);
				FTObj.DLEnd();
				FTObj.Cmd_Swap();
				data->readyFlag = 1;
				//delay(500);
			}
			else
			{
				this->drawRectangle(20, 220, 460, 260, CLR_HEATBOX);
				this->writeString_p(CLR_WHITE, 245, 240, DEFAULT_FONT, PRINT_HEATING_PREPARE);
				FTObj.DLEnd();
				FTObj.Cmd_Swap();
			}
			
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
			data->bed	 = (uint8_t)((degBed() / degTargetBed()) * 100.0F);
		}
		#endif
		
		#if ROBO_VERSION == ROBO_MINI
		void drawHeatBox()
		{
			this->drawRectangle((HEAT_BOX1_X + 135), HEAT_BOX_Y, ((HEAT_BOX1_X + 135) + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_HEATBOX);
			this->drawRectangle((HEAT_BOX1_X + 135), getPercentageHeight(data->nozel1), ((HEAT_BOX1_X + 135) + HEAT_BOX_WIDTH), (HEAT_BOX_HEIGHT + HEAT_BOX_Y), CLR_PANEL_PRESSED);
			this->writeString_p(CLR_BLACK, ((HEAT_BOX1_X + 135) + (HEAT_BOX_WIDTH / 2)), 201, HEAT_BOX_FONT, PRINT_HEATING_NOZELTEMP);
			if (data->nozel1 >= 100)
				this->writeString_p(CLR_WHITE, ((HEAT_BOX1_X + 135) + (HEAT_BOX_WIDTH / 2)), (HEAT_BOX_Y + (HEAT_BOX_HEIGHT / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			
			if (data->nozel1 == 100)
			{
				this->drawRectangle(20, 220, 460, 260, CLR_PANEL_PRESSED);
				this->writeString_p(CLR_WHITE, 245, 240, DEFAULT_FONT, PRINT_HEATING_READY);
				FTObj.DLEnd();
				FTObj.Cmd_Swap();
				data->readyFlag = 1;
				//delay(500);
			}
			else
			{
				this->drawRectangle(20, 220, 460, 260, CLR_HEATBOX);
				this->writeString_p(CLR_WHITE, 245, 240, DEFAULT_FONT, PRINT_HEATING_PREPARE);
				FTObj.DLEnd();
				FTObj.Cmd_Swap();
			}
			
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
		}
		#endif
		
		void clearTemp()
		{
			setTargetHotend(0, 0);
			setTargetHotend(0, 1);
			setTargetBed(0);
		}
};

#endif /* PRINT_HEATINGSCREEN_H_ */