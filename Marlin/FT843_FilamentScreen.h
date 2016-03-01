/*
 * FT843_FilamentScreen.h
 *
 * Created: 24/11/2014 11:05:24 PM
 *  Author: Sherif
 */ 


#ifndef FT843_FILAMENTSCREEN_H_
#define FT843_FILAMENTSCREEN_H_

#include "FT843_ScreenBase.h"

#define HEAT_BOX1_X		20
#define HEAT_BOX2_X		144
#define HEAT_BOX3_X		329
#define HEAT_BOX_WIDTH	90

#define FIL_LOAD	2
#define FIL_UNLOAD	4

#define FIL_STATUS_HEAT		0
#define FIL_STATUS_HEATING	1
#define FIL_STATUS_UNLOAD	2
#define FIL_STATUS_LOAD		3
#define FIL_STATUS_DONE		4

typedef struct FilamentScreenData
{
	Bitmap_header_t* back_home_header;
	
	uint32_t ptr_back_home;
	uint32_t ptr_back_home_click;
	
	uint8_t nozel1;
	uint8_t nozel2;
	
	uint8_t nozel1Ready;
	uint8_t nozel2Ready;
	
	uint8_t status;
	uint8_t screen;
	uint8_t preheatStarted;
	
	uint8_t load;
	
	uint32_t tubeStartTime;
	
	uint32_t ram_ptr;
	
}FilamentScreenData_t;

class FilamentScreen : public ScreenBase
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
			
			if (data->screen == 0)
			{
				drawLoadUnloadScreen(tag);
			}
			else if(data->screen == 1)
			{
				drawFilamentScreen(tag);
			}
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
		}
		
		void drawLoadUnloadScreen(uint8_t tag)
		{
			this->drawRectangle(20, 100, 230, 242, HEADER_COLOR, 2);
			if (tag == 2)
			{
				this->writeString_p(CLR_SPLIT_LINE, 125, 150, 30, FILAMENT_LOAD, 2);
				this->writeString_p(CLR_SPLIT_LINE, 125, 190, 30, FILAMENT_FILAMENT, 2);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 125, 150, 30, FILAMENT_LOAD, 2);
				this->writeString_p(CLR_WHITE, 125, 190, 30, FILAMENT_FILAMENT, 2);
			}
			
			this->drawRectangle(250, 100, 460, 242, HEADER_COLOR, 3);
			if (tag == 3)
			{
				this->writeString_p(CLR_SPLIT_LINE, 355, 150, 30, FILAMENT_UNLOAD, 3);
				this->writeString_p(CLR_SPLIT_LINE, 355, 190, 30, FILAMENT_FILAMENT, 3);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 355, 150, 30, FILAMENT_UNLOAD, 3);
				this->writeString_p(CLR_WHITE, 355, 190, 30, FILAMENT_FILAMENT, 3);
			}
		}
		
		#if ROBO_VERSION == ROBO_R2 || ROBO_VERSION == ROBO_MEGA || ROBO_VERSION == ROBO_R2_RAMPS
		void drawFilamentScreen(uint8_t tag)
		{
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
			data->nozel2 = (uint8_t)((degHotend(1) / degTargetHotend(1)) * 100.0F);
			
			if (data->nozel1 >= 100)
				data->nozel1Ready = 1;
			if (data->nozel2 >= 100)
				data->nozel2Ready = 1;
			
			
			if (tag == 1)
				drawBitmap(data->ptr_back_home_click, data->back_home_header, 10, 1, 255, 1);
			else
				drawBitmap(data->ptr_back_home, data->back_home_header, 10, 1, 255, 1);
			
			this->drawRectangle(HEAT_BOX1_X, 103, (HEAT_BOX1_X + HEAT_BOX_WIDTH), 223, CLR_HEATBOX, 4);
			this->writeString_p(CLR_BLACK, 65, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZEL1TEMP);
			
			this->drawRectangle(HEAT_BOX2_X, 103, (HEAT_BOX2_X + HEAT_BOX_WIDTH), 223, CLR_HEATBOX, 5);
			this->writeString_p(CLR_BLACK, 189, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZEL2TEMP);
			
			if (!(data->preheatStarted & 1))
			{
				writeClickToStart(65, 4);
			}
			else
			{
				this->drawRectangle(HEAT_BOX1_X, getPercentageHeight(data->nozel1), (HEAT_BOX1_X + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 4);
				if (data->nozel1Ready)
					this->writeString_p(CLR_WHITE, (HEAT_BOX1_X + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			if (!(data->preheatStarted & 2))
			{
				writeClickToStart(189, 5);
			}
			else
			{
				this->drawRectangle(HEAT_BOX2_X, getPercentageHeight(data->nozel2), (HEAT_BOX2_X + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 5);
				if (data->nozel2Ready)
					this->writeString_p(CLR_WHITE, (HEAT_BOX2_X + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			this->drawLine(275, 275, SPLIT_LINE_START, SCREEN_HEIGHT, CLR_SPLIT_LINE);
			
			writeStatusText();
			drawOkButton(tag);
		}
		#endif
		
		#if ROBO_VERSION == ROBO_R1 || ROBO_VERSION == ROBO_R1_PLUS || ROBO_VERSION == ROBO_MINI
		void drawFilamentScreen(uint8_t tag)
		{
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
			
			if (data->nozel1 >= 100)
				data->nozel1Ready = 1;
			if (data->nozel2 >= 100)
				data->nozel2Ready = 1;
			
			
			if (tag == 1)
				drawBitmap(data->ptr_back_home_click, data->back_home_header, 10, 1, 255, 1);
			else
				drawBitmap(data->ptr_back_home, data->back_home_header, 10, 1, 255, 1);
			
			this->drawRectangle((HEAT_BOX1_X + 60), 103, ((HEAT_BOX1_X + 60) + HEAT_BOX_WIDTH), 223, CLR_HEATBOX, 4);
			this->writeString_p(CLR_BLACK, 132, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZELTEMP);
			
			if (!(data->preheatStarted & 1))
			{
				writeClickToStart(129, 4);
			}
			else
			{
				this->drawRectangle((HEAT_BOX1_X + 60), getPercentageHeight(data->nozel1), ((HEAT_BOX1_X + 60) + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 4);
				if (data->nozel1Ready)
					this->writeString_p(CLR_WHITE, ((HEAT_BOX1_X + 60) + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			this->drawLine(275, 275, SPLIT_LINE_START, SCREEN_HEIGHT, CLR_SPLIT_LINE);
			
			writeStatusText();
			drawOkButton(tag);
		}
		#endif
		
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1: // Return Home
					this->clearTemp();
					return SCREEN_HOME;
				case 2: // Load Filament
					data->screen = 1;
					data->load = FIL_LOAD;
					this->tagPressed(0);
					break;
				case 3: // Unload Filament
					data->screen = 1;
					data->load = FIL_UNLOAD;
					this->tagPressed(0);
					break;
				case 4: // Nozel 1
					data->preheatStarted |= 1;
					data->status = data->status == FIL_STATUS_HEAT ? FIL_STATUS_HEATING : data->status;
					this->tagPressed(0);
					setTemp(0);
					break;
				case 5: // Nozel 2
					data->preheatStarted |= 2;
					data->status = data->status == FIL_STATUS_HEAT ? FIL_STATUS_HEATING : data->status;
					this->tagPressed(0);
					setTemp(1);
					break;
				case 6: // Ok button
					if (okPressed())
						return SCREEN_HOME;
					
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
			this->writeString_p(CLR_WHITE, SCREEN_WIDTH / 2, 36, 30, PSTR("FILAMENT"));
		}
		
		virtual uint8_t process()
		{
			this->tagPressed(0);
			
			if (data->status == FIL_STATUS_HEATING)
			{
				if (data->preheatStarted & 1)
				{
					if (data->preheatStarted & 2)
					{
						if (data->nozel1 >= 100 && data->nozel2 >= 100)
						{
							nozzlesHeated();
						}
					}
					else
					{
						if (data->nozel1 >= 100)
						{
							SERIAL_ECHO_START;
							SERIAL_ECHOLNPGM("Nozzle is heated");
							nozzlesHeated();
						}
					}
				}
				else if (data->preheatStarted & 2)
				{
					if (data->nozel2 >= 100)
					{
						nozzlesHeated();
					}
				}
			}
			else if (data->status == FIL_STATUS_LOAD)
			{
				if (millis() >= (data->tubeStartTime + 2950))
				{
					data->tubeStartTime = millis();
					extrude();
				}
			}
			else if (data->status == FIL_STATUS_UNLOAD)
			{
				if (millis() >= (data->tubeStartTime + 2950))
				{
					data->tubeStartTime = millis();
					retract();
				}
			}
			
			return 0;
		}
		
		void init()
		{
			SERIAL_ECHO_START;
			SERIAL_ECHOLNPGM("Filament screen");
			
			data = (FilamentScreenData_t*)malloc(sizeof(FilamentScreenData_t));
			data->ram_ptr = 0L;
			
			data->back_home_header = &Bitmap_headers[BH_BACK_HOME];
			
			data->nozel1 = 0;
			data->nozel2 = 0;
			
			data->nozel1Ready = 0;
			data->nozel2Ready = 0;
			
			data->preheatStarted = 0;
			data->status = 0;
			data->screen = 0;
			data->load = 0;
		}
		
		virtual void freeMem()
		{
			free(data);
		}
		
		FilamentScreenData_t* data;
	private:
		uint16_t getPercentageHeight(uint8_t perc)
		{
			perc = perc > 100 ? 100 : perc;
			float p = (float)perc / 100.0F;
			p = p * 119;
			p = (103 + 119) - p;
			return (uint16_t)p;
		}
		
		void writeClickToStart(uint16_t x, uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, x, 141, HEAT_BOX_FONT, PSTR("Press to"), tag);
			this->writeString_p(CLR_WHITE, x, 164, HEAT_BOX_FONT, PSTR("start"), tag);
			this->writeString_p(CLR_WHITE, x, 186, HEAT_BOX_FONT, PSTR("heating"), tag);
		}
		
		void setTemp(uint8_t end)
		{
			uint16_t hotEndTemp;
			hotEndTemp = ABS_PREHEAT_HOTEND_TEMP;
			setTargetHotend(hotEndTemp, end);
		}
		
		void clearTemp()
		{
			setTargetHotend(0, 0);
			setTargetHotend(0, 1);
			setTargetBed(0);
		}
		
		uint8_t okPressed()
		{
			if (data->status == FIL_STATUS_UNLOAD)
			{
				// Stop retracting
				data->status = FIL_STATUS_DONE;
				clearTemp();
			}
			else if (data->status == FIL_STATUS_LOAD)
			{
				// Stop Extruding
				data->status = FIL_STATUS_DONE;
			}
			else if (data->status == FIL_STATUS_DONE)
			{
				clearTemp();
				return 1;
			}
			
			tagPressed(0);
			return 0;
		}
		
		void writeStatusText()
		{
			if (data->status == FIL_STATUS_HEAT)
			{
				this->writeString_p(CLR_BLACK, 377, 110, DEFAULT_FONT, PSTR("Start heating"));
			}
			else if (data->status == FIL_STATUS_HEATING)
			{
				this->writeString_p(CLR_BLACK, 377, 110, DEFAULT_FONT, PSTR("Nozzle heating"));
			}
			else if (data->status == FIL_STATUS_UNLOAD)
			{
				this->writeString_p(CLR_BLACK, 377, 110, DEFAULT_FONT, PSTR("Retracting"));
				this->writeString_p(CLR_BLACK, 377, 135, DEFAULT_FONT, PSTR("Press Ok to stop"));
			}			
			else if (data->status == FIL_STATUS_LOAD)
			{
				this->writeString_p(CLR_BLACK, 377, 110, DEFAULT_FONT, PSTR("Extruding"));
				this->writeString_p(CLR_BLACK, 377, 135, DEFAULT_FONT, PSTR("Press Ok to stop"));
			}
			else if (data->status == FIL_STATUS_DONE)
			{
				this->writeString_p(CLR_BLACK, 377, 110, DEFAULT_FONT, PSTR("Done"));
			}
		}
		
		void drawOkButton(uint8_t tag)
		{
			if (data->status == FIL_STATUS_LOAD || data->status == FIL_STATUS_UNLOAD || data->status == FIL_STATUS_DONE)
			{
				this->drawRectangle(295, 155, 460, 195, HEADER_COLOR, 6);
				if (tag == 6)
				{
					if (data->status == FIL_STATUS_DONE)
					{
						this->writeString_p(CLR_SPLIT_LINE, 377, 175, DEFAULT_FONT, FILAMENT_RETURN_HOME, 6);
					}
					else
					{
						this->writeString_p(CLR_SPLIT_LINE, 377, 175, DEFAULT_FONT, FILAMENT_OK, 6);
					}
				}
				else
				{
					if (data->status == FIL_STATUS_DONE)
					{
						this->writeString_p(CLR_WHITE, 377, 175, DEFAULT_FONT, FILAMENT_RETURN_HOME, 6);
					}
					else
					{
						this->writeString_p(CLR_WHITE, 377, 175, DEFAULT_FONT, FILAMENT_OK, 6);
					}
				}
			}
			else
			{
				this->drawRectangle(295, 155, 460, 195, CLR_SPLIT_LINE);
				this->writeString_p(CLR_WHITE, 377, 175, DEFAULT_FONT, PSTR("Waiting..."));
			}
		}
		
		void nozzlesHeated()
		{
			if (data->load == FIL_LOAD)
			{
				// Extrude
				extrude();
				data->tubeStartTime = millis();
				data->status = FIL_STATUS_LOAD;
			}
			else if (data->load == FIL_UNLOAD)
			{
				// Retract
				retract();
				data->tubeStartTime = millis();
				data->status = FIL_STATUS_UNLOAD;
			}
		}
		
		void extrude()
		{
			enquecommand_P(PSTR("G91"));
			enquecommand_P(PSTR("G1 E5 F100"));
			enquecommand_P(PSTR("G90"));
		}
		
		void retract()
		{
			enquecommand_P(PSTR("G91"));
			enquecommand_P(PSTR("G1 E-5 F100"));
			enquecommand_P(PSTR("G90"));
		}
};



#endif /* FT843_FILAMENTSCREEN_H_ */