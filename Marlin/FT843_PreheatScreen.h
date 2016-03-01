/*
 * PreheatScreen.h
 *
 * Created: 4/11/2014 2:51:28 PM
 *  Author: Sherif
 */ 


#ifndef PREHEATSCREEN_H_
#define PREHEATSCREEN_H_


#include "FT843_ScreenBase.h"

#define MATERIAL_PLA 2
#define MATERIAL_ABS 1

#define CLR_HEATBOX COLOR_RGB(202, 202, 202)

#define HEAT_BOX1_X		59
#define HEAT_BOX2_X		194
#define HEAT_BOX3_X		329
#define HEAT_BOX_WIDTH	90

typedef struct PreheatScreenData
{
	uint32_t ptr_back_home;
	uint32_t ptr_back_home_click;
	
	uint8_t screen_mode;
	uint8_t material;
	
	uint8_t nozel1Started;
	uint8_t nozel2Started;
	uint8_t bedStarted;
	
	uint8_t nozel1 = 0;
	uint8_t nozel2 = 0;
	uint8_t bed = 0;
	
	Bitmap_header_t* back_home_header;
	uint32_t ram_ptr;
	
}PreheatScreenData_t;

class PreheatScreen : ScreenBase
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
			data->ptr_back_home = this->addImageInMemory(back_home_img, BACK_HOME_DATASIZE, BACK_HOME_INFLATED, data->back_home_header, &data->ram_ptr);
			data->ptr_back_home_click = this->addImageInMemory(back_home_click_img, BACK_HOME_CLICK_DATASIZE, BACK_HOME_INFLATED, data->back_home_header, &data->ram_ptr);
		}
		
		virtual void addHeader()
		{
			
		}
		
		virtual void drawHeader()
		{
			this->drawRectangle(0, 0, SCREEN_WIDTH, SPLIT_LINE_START - 1, HEADER_COLOR);
			this->writeString_p(CLR_WHITE, SCREEN_WIDTH / 2, 36, 30, PSTR("PRE HEAT"));
		}
		
		virtual void tagPressed(uint8_t tag)
		{
			FTObj.DLStart();
			this->clear();
			this->drawHeader();
			
			if (tag == 1)
				drawBitmap(data->ptr_back_home_click, data->back_home_header, 10, 1, 255, 1);
			else
				drawBitmap(data->ptr_back_home, data->back_home_header, 10, 1, 255, 1);
			
			if (data->screen_mode == 0)
			{
				materialScreen(tag);
			}
			else if (data->screen_mode == 1)
			{
				preheatScreen(tag);
			}
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
		}
		
		void materialScreen(uint8_t tag)
		{
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 105, 28, PSTR("Choose your material"));
			
			this->drawRectangle(34, 136, 224, 242, HEADER_COLOR, 2);
			if (tag == 2)
			{
				this->writeString_p(CLR_SPLIT_LINE, 101, 190, 30, PREHEAT_PLA, FT_OPT_CENTERY, 2);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 101, 190, 30, PREHEAT_PLA, FT_OPT_CENTERY, 2);
			}
			
			this->drawRectangle(255, 136, 445, 242, HEADER_COLOR, 3);
			if (tag == 3)
			{
				this->writeString_p(CLR_SPLIT_LINE, 320, 190, 30, PREHEAT_ABS, FT_OPT_CENTERY, 3);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 320, 190, 30, PREHEAT_ABS, FT_OPT_CENTERY, 3);
			}
		}
		
		#if ROBO_VERSION == ROBO_R2 || ROBO_VERSION == ROBO_MEGA || ROBO_VERSION == ROBO_R2_RAMPS
		void preheatScreen(uint8_t tag)
		{
			
			this->drawRectangle(HEAT_BOX1_X, 103, (HEAT_BOX1_X + HEAT_BOX_WIDTH), 223, CLR_HEATBOX, 4);
			this->writeString_p(CLR_BLACK, 104, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZEL1TEMP);
			
			this->drawRectangle(HEAT_BOX2_X, 103, 284, 223, CLR_HEATBOX, 5);
			this->writeString_p(CLR_BLACK, 239, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZEL2TEMP);
			
			this->drawRectangle(HEAT_BOX3_X, 103, 419, 223, CLR_HEATBOX, 6);
			this->writeString_p(CLR_BLACK, 374, 240, HEAT_BOX_FONT, PRINT_HEATING_BEDTEMP);
			
			if (!data->nozel1Started)
			{
				writeClickToStart(104, 4);
			}
			else
			{
				this->drawRectangle(HEAT_BOX1_X, getPercentageHeight(data->nozel1), (HEAT_BOX1_X + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 4);
				if (data->nozel1 >= 100)
					this->writeString_p(CLR_WHITE, (HEAT_BOX1_X + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			if (!data->nozel2Started)
			{
				writeClickToStart(239, 5);
			}
			else
			{
				this->drawRectangle(HEAT_BOX2_X, getPercentageHeight(data->nozel2), (HEAT_BOX2_X + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 5);
				if (data->nozel2 >= 100)
					this->writeString_p(CLR_WHITE, (HEAT_BOX2_X + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			if (!data->bedStarted)
			{
				writeClickToStart(374, 6);
			}
			else
			{
				this->drawRectangle(HEAT_BOX3_X, getPercentageHeight(data->bed),	(HEAT_BOX3_X + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 6);
				if (data->bed >= 100)
					this->writeString_p(CLR_WHITE, (HEAT_BOX3_X + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
			data->nozel2 = (uint8_t)((degHotend(1) / degTargetHotend(1)) * 100.0F);
			data->bed	 = (uint8_t)((degBed() / degTargetBed()) * 100.0F);
		}
		#endif
		
		#if ROBO_VERSION == ROBO_R1 || ROBO_VERSION == ROBO_R1_PLUS
		void preheatScreen(uint8_t tag)
		{
			
			this->drawRectangle((HEAT_BOX1_X + 50), 103, ((HEAT_BOX1_X + 50) + HEAT_BOX_WIDTH), 223, CLR_HEATBOX, 4);
			this->writeString_p(CLR_BLACK, 154, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZEL1TEMP);
			
			this->drawRectangle((HEAT_BOX3_X - 50), 103, (((HEAT_BOX3_X - 50) + HEAT_BOX_WIDTH)), 223, CLR_HEATBOX, 6);
			this->writeString_p(CLR_BLACK, 324, 240, HEAT_BOX_FONT, PRINT_HEATING_BEDTEMP);
			
			if (!data->nozel1Started)
			{
				writeClickToStart(154, 4);
			}
			else
			{
				this->drawRectangle((HEAT_BOX1_X + 50), getPercentageHeight(data->nozel1), ((HEAT_BOX1_X + 50) + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 4);
				if (data->nozel1 >= 100)
					this->writeString_p(CLR_WHITE, ((HEAT_BOX1_X + 50) + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			if (!data->bedStarted)
			{
				writeClickToStart(324, 6);
			}
			else
			{
				this->drawRectangle((HEAT_BOX3_X - 50), getPercentageHeight(data->bed),	((HEAT_BOX3_X - 50) + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 6);
				if (data->bed >= 100)
					this->writeString_p(CLR_WHITE, ((HEAT_BOX3_X - 50) + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
			data->bed	 = (uint8_t)((degBed() / degTargetBed()) * 100.0F);
		}
		#endif
		
		#if ROBO_VERSION == ROBO_MINI
		void preheatScreen(uint8_t tag)
		{
			
			this->drawRectangle(HEAT_BOX2_X, 103, (HEAT_BOX2_X + HEAT_BOX_WIDTH), 223, CLR_HEATBOX, 4);
			this->writeString_p(CLR_BLACK, 239, 240, HEAT_BOX_FONT, PRINT_HEATING_NOZELTEMP);
			
			if (!data->nozel1Started)
			{
				writeClickToStart(239, 4);
			}
			else
			{
				this->drawRectangle(HEAT_BOX2_X, getPercentageHeight(data->nozel1), (HEAT_BOX2_X + HEAT_BOX_WIDTH), 223, CLR_PANEL_PRESSED, 4);
				if (data->nozel1 >= 100)
				this->writeString_p(CLR_WHITE, (HEAT_BOX2_X + (HEAT_BOX_WIDTH / 2)), (103 + (120 / 2)), HEAT_BOX_FONT, PRINT_HEATING_READY);
			}
			
			data->nozel1 = (uint8_t)((degHotend(0) / degTargetHotend(0)) * 100.0F);
		}
		#endif
		
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 1:
					clearTemp();
					return SCREEN_HOME;
				case 2:
					data->screen_mode = 1;
					data->material = MATERIAL_PLA;
					this->tagPressed(0);
					break;
				case 3:
					data->screen_mode = 1;
					data->material = MATERIAL_ABS;
					this->tagPressed(0);
					break;
				case 4: // Nozel 1
					data->nozel1Started = 127;
					this->tagPressed(0);
					setTemp(0);
					break;
				case 5: // Nozel 2
					data->nozel2Started = 127;
					this->tagPressed(0);
					setTemp(1);
					break;
				case 6: // Bed
					data->bedStarted = 127;
					this->tagPressed(0);
					setTemp(2);
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
			SERIAL_ECHO_START;
			SERIAL_ECHOLNPGM("Preheat screen");
			data = (PreheatScreenData_t*)malloc(sizeof(PreheatScreenData_t));
			
			data->ram_ptr = 0L;
			data->back_home_header = &Bitmap_headers[BH_BACK_HOME];
			data->screen_mode = 0;
			
			data->nozel1 = 0;
			data->nozel2 = 0;
			data->bed = 0;
			
			data->nozel1Started = 0;
			data->nozel2Started = 0;
			data->bedStarted = 0;
		}
		
		PreheatScreenData_t* data;
		
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
			this->writeString_p(CLR_WHITE, x, 186, HEAT_BOX_FONT, PSTR("pre heat"), tag);
		}
		
		void setTemp(uint8_t end)
		{
			uint16_t hotEndTemp;
			uint16_t bed;
			if (data->material == MATERIAL_ABS)
			{
				hotEndTemp = ABS_PREHEAT_HOTEND_TEMP;
				bed = ABS_PREHEAT_HPB_TEMP;
			}
			else if (data->material == MATERIAL_PLA)
			{
				hotEndTemp = PLA_PREHEAT_HOTEND_TEMP;
				bed = PLA_PREHEAT_HPB_TEMP;
			}
			else
			{
				return;
			}
			
			if (end == 0 || end == 1)
			{
				setTargetHotend(hotEndTemp, end);
			}
			else
			{
				setTargetBed(bed);
			}
		}
		
		void clearTemp()
		{
			setTargetHotend(0, 0);
			setTargetHotend(0, 1);
			setTargetBed(0);
		}
};


#endif /* PREHEATSCREEN_H_ */