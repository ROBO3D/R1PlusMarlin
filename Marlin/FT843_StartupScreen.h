/*
 * FT843_StartupScreen.h
 *
 * Created: 20/11/2014 11:28:15 PM
 *  Author: Sherif
 */ 


#ifndef FT843_STARTUPSCREEN_H_
#define FT843_STARTUPSCREEN_H_

#include "FT843_ScreenBase.h"

typedef struct StartupScreenData
{	
	uint32_t ptr_down;
	uint32_t ptr_down_click;
	uint32_t ptr_up;
	uint32_t ptr_up_click;
	
	uint8_t screenCount;
	uint8_t bedMCount;
	
	uint8_t pressStarted;
	uint32_t pressTime;
	
	float currentZ;
	float rate;
	
	uint32_t ram_ptr;
	Bitmap_header_t* arrows_header;
	
}StartupScreenData_t;

class StartupScreen : public ScreenBase
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
			data->ptr_up = this->addImageInMemory(up_img, UP_DATASIZE, UP_INFLATED, data->arrows_header, &data->ram_ptr);
			data->ptr_up_click = this->addImageInMemory(up_click_img, UP_CLICK_DATASIZE, UP_INFLATED, data->arrows_header, &data->ram_ptr);
			
			data->ptr_down = this->addImageInMemory(down_img, DOWN_DATASIZE, DOWN_INFLATED, data->arrows_header, &data->ram_ptr);
			data->ptr_down_click = this->addImageInMemory(down_click_img, DOWN_CLICK_DATASIZE, DOWN_INFLATED, data->arrows_header, &data->ram_ptr);
		}
		
		virtual void tagPressed(uint8_t tag)
		{	
			FTObj.Cmd_DLStart();
			this->clear();
			this->drawHeader();
			
			if (data->screenCount == 1)
			{				
				screenOne(tag);
			}
			else if (data->screenCount == 2)
			{				
				screenTwo(tag);
			}
			else if (data->screenCount == 3)
			{
				screenThree(tag);
			}
			else if (data->screenCount == 4)
			{
				screenFour(tag);
			}
			else if (data->screenCount == 5)
			{
				screenFive(tag);
			}
			else if (data->screenCount == 6)
			{
				screenSix(tag);
			}
			else if (data->screenCount == 7)
			{
				screenSeven(tag);
			}
			else if (data->screenCount == 8)
			{
				screenEight(tag);
			}
			
			
			FTObj.DLEnd();
			FTObj.Cmd_Swap();
		}
		
		virtual uint8_t tagReleased(uint8_t tag)
		{
			switch (tag)
			{
				case 2: // First screen Continue
					data->screenCount = 2;
					enquecommand_P(PSTR("G1 F2000"));
					enquecommand_P(PSTR("G28"));
					
					#if ROBO_VERSION == ROBO_R2
					enquecommand_P(PSTR("G1 X126 Y260"));
					#elif ROBO_VERSION == ROBO_MINI
					enquecommand_P(PSTR("G1 X55 Y105"));
					#endif
					
					this->moveBed(data->currentZ);
					this->tagPressed(0);
					break;
				case 3: // Second screen Continue
					data->screenCount = 3;
					this->moveBed(data->currentZ + 2.0F);
					enquecommand_P(PSTR("G1 X0 Y0"));
					this->moveBed(data->currentZ);
					this->tagPressed(0);
					// Move nozzle to position 2
					break;
				case 4: // Second screen up
					// Make bed go up
					data->pressStarted = 0;
					this->tagPressed(0);
					break;
				case 5: // Second screen down
					// Make bed go down
					data->pressStarted = 0;
					this->tagPressed(0);
					break;
				case 6: // Third screen Continue
					data->screenCount = 4;
					this->moveBed(data->currentZ + 2.0F);
					#if ROBO_VERSION == ROBO_R2
					enquecommand_P(PSTR("G1 X254 Y0"));
					#elif ROBO_VERSION == ROBO_MINI
					enquecommand_P(PSTR("G1 X110 Y0"));
					#endif
					
					this->moveBed(data->currentZ);
					this->tagPressed(0);
					// Move Nozzle to position 3
					break;
				case 7: // Fourth screen Continue
					data->screenCount = 5;
					this->moveBed(data->currentZ + 2.0F);
					#if ROBO_VERSION == ROBO_R2
					enquecommand_P(PSTR("G1 X126 Y260"));
					#elif ROBO_VERSION == ROBO_MINI
					enquecommand_P(PSTR("G1 X55 Y105"));
					#endif
					this->moveBed(data->currentZ);
					this->tagPressed(0);
					// Move Nozzle to position 4
					break;
				case 8: // Fifth screen Continue
					data->screenCount = 6;
					this->moveBed(data->currentZ + 2.0F);
					enquecommand_P(PSTR("G1 X0 Y0"));
					this->moveBed(data->currentZ);
					this->tagPressed(0);
					break;
				case 9: // Sixth screen Continue
					data->screenCount = 7;
					this->moveBed(data->currentZ + 2.0F);
					#if ROBO_VERSION == ROBO_R2
					enquecommand_P(PSTR("G1 X254 Y0"));
					#elif ROBO_VERSION == ROBO_MINI
					enquecommand_P(PSTR("G1 X110 Y0"));
					#endif
					this->moveBed(data->currentZ);
					this->tagPressed(0);
					break;
				case 10: // Seventh screen Done
					data->screenCount = 8;
					this->tagPressed(0);
					add_homeing[Z_AXIS] = data->currentZ;
					setStartupSequence('R');
					Config_StoreSettings();
					delay(3000);
					enquecommand_P(PSTR("G28"));
					return SCREEN_HOME;
			}
			
			return SCREEN_SPLASH;
		}
		
		virtual void addHeader()
		{
			
		}
		
		virtual void drawHeader()
		{
			this->drawRectangle(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, HEADER_COLOR);
			this->writeString_p(CLR_WHITE, SCREEN_WIDTH / 2, 36, 30, PSTR("Startup"));
		}
		
		virtual uint8_t process()
		{
			this->tagPressed(0);
			return 0;
		}
		
		void init()
		{
			data = (StartupScreenData_t*)malloc(sizeof(StartupScreenData_t));
			
			data->ram_ptr = 0L;
			
			data->arrows_header	= &Bitmap_headers[BH_ARROWS];
			data->screenCount = 1;
			data->currentZ = 20.0F;
			
			data->pressStarted = 0;
			data->pressTime = 0;
			
			data->bedMCount = 0;
		}
		
		virtual void freeMem()
		{
			free(data);
		}
		
		StartupScreenData_t* data;
		
	private:
		void screenOne(uint8_t tag)
		{			
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 100, DEFAULT_FONT, PSTR("Welcome to your new Robo3D R2"));
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 130, DEFAULT_FONT, PSTR("We are now going to run you through setting up"));
			this->writeString_p(CLR_BLACK, SCREEN_WIDTH / 2, 160, DEFAULT_FONT, PSTR("your printer. Press Continue to start."));
			
			this->drawRectangle(100, 195, 380, 260, HEADER_COLOR, 2);
			if (tag == 2)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 227, 30, STARTUP_CONTINUE, 2);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 227, 30, STARTUP_CONTINUE, 2);
			}
			
			//if (data->screenCount != 0)
			//{
				//data->screenCount = 0;
			//}
		}
		
		void screenTwo(uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, 400, HEADER_HEIGHT / 2, DEFAULT_FONT, PSTR("Step 1 of 6"));
			
			this->writeString_p(CLR_BLACK, 200, 100, DEFAULT_FONT, PSTR("We are going to level the build plate."));
			this->writeString_p(CLR_BLACK, 200, 130, DEFAULT_FONT, PSTR("Press the up or down arrows until your"));
			this->writeString_p(CLR_BLACK, 200, 160, DEFAULT_FONT, PSTR("nozzle is 1mm above the build plate"));
			
			this->drawRectangle(70, 195, 325, 260, HEADER_COLOR, 3);
			if (tag == 3)
			{
				this->writeString_p(CLR_SPLIT_LINE, 197, 227, 30, STARTUP_CONTINUE, 3);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 197, 227, 30, STARTUP_CONTINUE, 3);
			}
			
			this->drawLine(405, 405, SPLIT_LINE_START, SCREEN_HEIGHT, CLR_SPLIT_LINE);
			
			this->drawRectangle(411, 80, SCREEN_WIDTH, 135, CLR_WHITE, 4);
			if (tag == 4)
			{
				// Make bed go up
				upButtonPressed();
				drawBitmap(data->ptr_up_click, data->arrows_header, 425, 95, 255, 4);
			}
			else
			{
				drawBitmap(data->ptr_up, data->arrows_header, 425, 95, 255, 4);
			}
			
			this->drawRectangle(411, 220, SCREEN_WIDTH, SCREEN_HEIGHT, CLR_WHITE, 5);
			if (tag == 5)
			{
				// Make bed go down
				downButtonPressed();
				drawBitmap(data->ptr_down_click, data->arrows_header, 425, 220, 255, 5);
			}
			else
			{
				drawBitmap(data->ptr_down, data->arrows_header, 425, 220, 255, 5);
			}
		}
		
		void screenThree(uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, 400, HEADER_HEIGHT / 2, DEFAULT_FONT, PSTR("Step 2 of 6"));
			
			this->writeString_p(CLR_BLACK, 240, 100, DEFAULT_FONT, PSTR("Twist the left leveling screw until the nozzle"));
			this->writeString_p(CLR_BLACK, 240, 130, DEFAULT_FONT, PSTR("nozzle is 1mm away from the build plate."));
			this->writeString_p(CLR_BLACK, 240, 160, DEFAULT_FONT, PSTR("Then press continue"));
			
			this->drawRectangle(100, 195, 380, 260, HEADER_COLOR, 6);
			if (tag == 6)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 227, 30, STARTUP_CONTINUE, 6);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 227, 30, STARTUP_CONTINUE, 6);
			}
		}
		
		void screenFour(uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, 400, HEADER_HEIGHT / 2, DEFAULT_FONT, PSTR("Step 3 of 6"));
			
			this->writeString_p(CLR_BLACK, 240, 100, DEFAULT_FONT, PSTR("Twist the right leveling screw until the nozzle"));
			this->writeString_p(CLR_BLACK, 240, 130, DEFAULT_FONT, PSTR("nozzle is 1mm away from the build plate."));
			this->writeString_p(CLR_BLACK, 240, 160, DEFAULT_FONT, PSTR("Then press continue"));
			
			this->drawRectangle(100, 195, 380, 260, HEADER_COLOR, 7);
			if (tag == 7)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 227, 30, STARTUP_CONTINUE, 7);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 227, 30, STARTUP_CONTINUE, 7);
			}
		}
		
		void screenFive(uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, 400, HEADER_HEIGHT / 2, DEFAULT_FONT, PSTR("Step 4 of 6"));
			
			this->writeString_p(CLR_BLACK, 240, 100, DEFAULT_FONT, PSTR("Now repeat this step but use a sheet of paper to"));
			this->writeString_p(CLR_BLACK, 240, 130, DEFAULT_FONT, PSTR("fine tune your build plate, slide the paper between"));
			this->writeString_p(CLR_BLACK, 240, 160, DEFAULT_FONT, PSTR("the nozzle and the build plate until there is some"));
			this->writeString_p(CLR_BLACK, 240, 190, DEFAULT_FONT, PSTR("resistance while using the thumb to level it"));
			
			this->drawRectangle(100, 210, 380, 265, HEADER_COLOR, 8);
			if (tag == 8)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 237, 30, STARTUP_CONTINUE, 8);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 237, 30, STARTUP_CONTINUE, 8);
			}
		}
		
		void screenSix(uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, 400, HEADER_HEIGHT / 2, DEFAULT_FONT, PSTR("Step 5 of 6"));
			
			this->writeString_p(CLR_BLACK, 240, 100, DEFAULT_FONT, PSTR("Make sure there is a small bit of resistance while"));
			this->writeString_p(CLR_BLACK, 240, 130, DEFAULT_FONT, PSTR("using the sheet of paper to fine tune your"));
			this->writeString_p(CLR_BLACK, 240, 160, DEFAULT_FONT, PSTR("left thumb screw level"));
			
			this->drawRectangle(100, 200, 380, 260, HEADER_COLOR, 9);
			if (tag == 9)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 230, 30, STARTUP_CONTINUE, 9);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 230, 30, STARTUP_CONTINUE, 9);
			}
		}
		
		void screenSeven(uint8_t tag)
		{
			this->writeString_p(CLR_WHITE, 400, HEADER_HEIGHT / 2, DEFAULT_FONT, PSTR("Step 6 of 6"));
			
			this->writeString_p(CLR_BLACK, 240, 100, DEFAULT_FONT, PSTR("Now do the same for the right thumb screw."));
			this->writeString_p(CLR_BLACK, 240, 130, DEFAULT_FONT, PSTR("Then press done."));
			
			this->drawRectangle(100, 200, 380, 260, HEADER_COLOR, 10);
			if (tag == 10)
			{
				this->writeString_p(CLR_SPLIT_LINE, 240, 230, 30, STARTUP_DONE, 10);
			}
			else
			{
				this->writeString_p(CLR_WHITE, 240, 230, 30, STARTUP_DONE, 10);
			}
		}
		
		void screenEight(uint8_t tag)
		{
			this->writeString_p(CLR_BLACK, 240, 100, DEFAULT_FONT, PSTR("Congratulations, you are now ready to use"));
			this->writeString_p(CLR_BLACK, 240, 130, DEFAULT_FONT, PSTR("your Robo3D R2 3D printer!"));
		}
		
		void moveBed(float z)
		{
			char str[12];
			memset(str, 0, 12);
			
			int wholeNumber = z;
			int decimalValue = (int)((z - (float)wholeNumber) * 10);
			
			sprintf_P(str, PSTR("G1 Z%d.%d"), wholeNumber, decimalValue);
			enquecommand(str);
		}
		
		void upButtonPressed()
		{
			if (!data->pressStarted)
			{
				data->bedMCount = 0;
				data->pressStarted = 1;
				data->rate = -0.5F;
				
				data->currentZ += data->rate;
				this->moveBed(data->currentZ);
				data->pressTime = millis();
			}
			
			if (millis() >= (data->pressTime + 200))
			{
				data->bedMCount++;
				data->currentZ += data->rate;
				this->moveBed(data->currentZ);
				data->pressTime = millis();
				if (data->bedMCount >= 5)
				{
					
				}
			}
		}
		
		void downButtonPressed()
		{
			if (!data->pressStarted)
			{
				data->bedMCount = 0;
				data->pressStarted = 1;
				data->rate = 0.5F;
				
				data->currentZ += data->rate;
				this->moveBed(data->currentZ);
				data->pressTime = millis();
			}
			
			if (millis() >= (data->pressTime + 200))
			{
				data->bedMCount++;
				data->currentZ += data->rate;
				this->moveBed(data->currentZ);
				data->pressTime = millis();
				if (data->bedMCount >= 5)
				{
					
				}
			}
		}
};



#endif /* FT843_STARTUPSCREEN_H_ */