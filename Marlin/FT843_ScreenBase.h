/*
 * ScreenBase.h
 *
 * Created: 15/10/2014 8:43:25 AM
 *  Author: Sherif
 */ 

#ifndef SCREENBASE_H_
#define SCREENBASE_H_

#include <Arduino.h>
#include "FT_Breakout_4DLCD_FT843.h"
#include "FT843_Images.h"
#include "FT843_Strings.h"
#include "Marlin.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define SCREEN_SPLASH		 0
#define SCREEN_HOME			 1
#define SCREEN_PRINT_SDCARD  2
#define SCREEN_PRINT_HEATING 3
#define SCREEN_PRINTING		 4
#define SCREEN_PREHEAT		 5
#define SCREEN_STARTUP		 6
#define SCREEN_FILAMENT		 7
#define SCREEN_UTILITIES	 8
#define SCREEN_SETTINGS		 9

#define DEFAULT_FONT 28
#define FT_OPT_3D 0

#define HEADER_COLOR COLOR(35, 116, 237)
#define HEADER_HEIGHT 72

/* Split line between panels color and height */
#define CLR_SPLIT_LINE 0xD1D4D8
#define SPLIT_LINE_START (HEADER_HEIGHT + 1)

#define COLOR(R, G, B) ((R << 16) | (G << 8) | (B))
#define CLR_LINE_HEADER COLOR_RGB(32, 86, 207)
#define CLR_WHITE COLOR_RGB(255, 255, 255)
#define CLR_BLACK COLOR_RGB(1, 1, 1)
#define CLR_PANEL_PRESSED HEADER_COLOR

#define DEBUG_PRINT(x, y) Serial.print(x); Serial.print(y);
#define DEBUG_PRINTLN(x, y) Serial.print(x); Serial.println(y);

/* Bitmap headers definitions, those contain the resolution color format and so forth */
#define BH_SPLASH			0
#define BH_HOME_HEADER		1
#define BH_HOME_SCREEN		2
#define BH_PRINT_HEADER		3
#define BH_ARROWS			4
#define BH_BACK_HOME		5
#define BH_BACK_SDCARD		6
#define BH_PRINTING_HEADER	7
#define BH_PRINTING_PAUSED	8
#define BH_PRINT_CANCELED	9
#define BH_RIGHT_PRINT		10

typedef struct Bitmap_header
{
	uint8_t Format;
	uint16_t Width;
	uint16_t Height;
	uint16_t Stride;
	uint32_t Arrayoffset;
}Bitmap_header_t;

class ScreenBase
{
	public:
		
		/* Adds interface controls to the display list without ending and swapping frames, allows to blend multiple screens together */
		virtual void addControls(uint8_t alpha) = 0;
		
		/* Fully displays the screen */
		virtual void displayControls(uint8_t alpha) = 0;
		
		/* Loads any Progmem image */
		virtual void loadImages() = 0;
		
		/* When the user is holding down and pressing on a tag */
		virtual void tagPressed(uint8_t tag) = 0;
		
		/* When the tag is pressed and released (Execute the function) */
		virtual uint8_t tagReleased(uint8_t tag) = 0;
		
		/* Inflates a binh image from progmem onto ram of FT800 */
		void inflateImage(prog_uchar* ptr, uint32_t dataSize, Bitmap_header_t* logoHeader, uint32_t ram_ptr);
		
		/* Adds top header to be added on all screens*/
		virtual void addHeader() = 0;
		
		/* Adds top header to be added on all screens*/
		virtual void drawHeader() = 0;
		
		/* Frees allocated memory for the object's data */
		virtual void freeMem() = 0;
		
		/* Processes the screen data in loop() in case it requires it */
		virtual uint8_t process() = 0;
		
		/* Draws bitmap given ram location (Called after inflation) */
		void drawBitmap(uint32_t ramLocation, Bitmap_header_t* header, uint16_t x, uint16_t y, uint8_t alpha);
		
		/* Draws bitmap with a tag given ram location (Called after inflation) */
		void drawBitmap(uint32_t ramLocation, Bitmap_header_t* header, uint16_t x, uint16_t y, uint8_t alpha, uint8_t tag);
		
		/* Draws a line with a color */
		void drawLine(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint32_t color);
		
		/* Writes a string at a given location */
		void writeString(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str);
		
		/* Writes a string at a given location */
		void writeString(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint8_t tag);
		
		/* Writes a string at a given location */
		void writeString(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint16_t options, uint8_t tag);
		
		/* Writes a string at a given location from program memory */
		void writeString_p(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str);
		
		/* Writes a string at a given location from program memory */
		void writeString_p(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint8_t tag);
		
		/* Writes a string at a given location from program memory */
		void writeString_p(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint16_t options, uint8_t tag);
		
		/* Clear colors */
		void clear();
		
		/* Bootup the FT800 and configure it */
		int16_t BootupConfigure();
		
		/* Reads currently pressed tag */
		uint8_t read_tags(uint8_t* eventFlag);
		
		/* Draws a rectangle at the coordinates given */
		void drawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
		
		/* Draws a rectangle at the coordinates given with a tag */
		void drawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color, uint8_t tag);
		
		/* Draws the grey lines between the panels */
		void drawSplitLines(uint8_t number);
		
		/* Clears the screen */
		void clearScreen();
		
		/* Calibrates the touch screen */
		void Calibrate();
		
		/* adds image in memory of FT800 while keeping track of its ram ptr */
		uint32_t addImageInMemory(const uint8_t* img, uint32_t dataSize, uint32_t inflated, Bitmap_header_t* imgHeader, uint32_t* ram_ptr);
		
		/* Headers for all progmem images */
		static Bitmap_header_t Bitmap_headers[];
		
		/* Object for FT800 Implementation */
		static FT800IMPL_SPI FTObj;
};



#endif /* SCREENBASE_H_ */
