/*
 * ScreenBase.cpp
 *
 * Created: 15/10/2014 8:40:37 AM
 *  Author: Sherif
 */ 
#include "FT843_ScreenBase.h"

void ScreenBase::inflateImage(prog_uchar* ptr, uint32_t dataSize, Bitmap_header_t* imgHeader, uint32_t ram_ptr)
{
	// Clear memory at the location
	FTObj.Cmd_Memset(ram_ptr, 0, 1L * imgHeader->Stride * imgHeader->Height);
	
	// Inflate the progmem of the image
	uint32_t ret = FTObj.Cmd_Inflate(ram_ptr);
	
	//DEBUG_PRINT("ram_ptr: ", ram_ptr);
	//DEBUG_PRINTLN("\t datasize: ", dataSize);
	
	// Write the logo
	FTObj.WriteCmdfromflash(ptr, dataSize);
}

int16_t ScreenBase::BootupConfigure()
{
	while(true)
	{
		uint32_t chipid = 0;
		
		FTObj.Init(FT_DISPLAY_RESOLUTION);//configure the display to the WQVGA
		
		delay(20);//for safer side
		chipid = FTObj.Read32(FT_ROM_CHIPID);

		/* Identify the chip */
		if(FT800_CHIPID != chipid)
		{
			SERIAL_ECHO_START;
			SERIAL_ECHOLN(chipid);
			delay(800);
			continue;
		}
		
		break;
	}
	
	
	/* Set the Display & audio pins */
	FTObj.SetDisplayEnablePin(FT_DISPENABLE_PIN);
	FTObj.SetAudioEnablePin(FT_AUDIOENABLE_PIN);
	FTObj.DisplayOn();
	FTObj.AudioOn();
	Calibrate();
	return 1;
}

void ScreenBase::drawLine(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint32_t color)
{
	FTObj.Begin(FT_LINES);
	FTObj.ColorRGB(color);
	FTObj.Vertex2ii(x1, y1);
	FTObj.Vertex2ii(x2, y2);
	FTObj.ColorRGB(CLR_WHITE);
}

void ScreenBase::drawBitmap(uint32_t ramLocation, Bitmap_header_t* header, uint16_t x, uint16_t y, uint8_t alpha)
{
	FTObj.Begin(FT_BITMAPS);
	FTObj.BitmapSource(ramLocation);
	FTObj.BitmapLayout(header->Format, header->Stride, header->Height);
	FTObj.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, header->Width, header->Height);
	FTObj.ColorA(alpha);
	FTObj.Vertex2ii(x, y);
}

void ScreenBase::drawBitmap(uint32_t ramLocation, Bitmap_header_t* header, uint16_t x, uint16_t y, uint8_t alpha, uint8_t tag)
{
	FTObj.Begin(FT_BITMAPS);
	FTObj.BitmapSource(ramLocation);
	FTObj.BitmapLayout(header->Format, header->Stride, header->Height);
	FTObj.BitmapSize(FT_NEAREST, FT_BORDER, FT_BORDER, header->Width, header->Height);
	FTObj.ColorA(alpha);
	FTObj.Tag(tag);
	FTObj.Vertex2ii(x, y);
	FTObj.Tag(0);
}

void ScreenBase::writeString(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str)
{
	FTObj.ColorRGB(color);
	FTObj.Cmd_Text(x, y, font, FT_OPT_CENTER, str);
}

void ScreenBase::writeString(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint8_t tag)
{
	FTObj.ColorRGB(color);
	FTObj.Tag(tag);
	FTObj.Cmd_Text(x, y, font, FT_OPT_CENTER, str);
	FTObj.Tag(0);
}

void ScreenBase::writeString(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint16_t options, uint8_t tag)
{
	FTObj.ColorRGB(color);
	FTObj.Tag(tag);
	FTObj.Cmd_Text(x, y, font, FT_OPT_CENTERY, str);
	FTObj.Tag(0);
}

void ScreenBase::writeString_p(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str)
{
	char* arr = (char*)malloc(32);
	strcpy_P(arr, str);
	FTObj.ColorRGB(color);
	FTObj.Cmd_Text(x, y, font, FT_OPT_CENTER, arr);
	free(arr);
}

void ScreenBase::writeString_p(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint8_t tag)
{
	char* arr = (char*)malloc(32);
	strcpy_P(arr, str);
	FTObj.ColorRGB(color);
	FTObj.Tag(tag);
	FTObj.Cmd_Text(x, y, font, FT_OPT_CENTER, arr);
	FTObj.Tag(0);
	free(arr);
}

void ScreenBase::writeString_p(uint32_t color, uint16_t x, uint16_t y, uint16_t font, const char* str, uint16_t options, uint8_t tag)
{
	char* arr = (char*)malloc(32);
	strcpy_P(arr, str);
	FTObj.ColorRGB(color);
	FTObj.Tag(tag);
	FTObj.Cmd_Text(x, y, font, FT_OPT_CENTERY, arr);
	FTObj.Tag(0);
	free(arr);
}

void ScreenBase::clear()
{
	FTObj.ClearColorRGB(255, 255, 255);
	FTObj.Clear(1, 1, 1);
}

uint8_t ScreenBase::read_tags(uint8_t* eventFlag)
{
	static uint8_t read_tag = 0, pressedFlag = 0, ret_tag = 0;
	static uint32_t timeOfPress;
	static uint16_t arr[12];
	
	read_tag = FTObj.Read(REG_TOUCH_TAG);
	if(read_tag != NULL && read_tag != 255)
	{
		if (!pressedFlag)
		{
			for (int i = 0; i < 12; i++)
				arr[i] = 0;
				
			ret_tag = 0;
			pressedFlag = 1;
			timeOfPress = millis();
		}
		
		if (read_tag < 12)
		{
			arr[read_tag - 1]++;
		}
		
		// Accumulate press values and get the highest number reported for the tag, reduces noise
		if ((millis() - timeOfPress) >= 20)
		{
			*eventFlag = 0;
			uint8_t arrID = 0;
			for (int i = 1; i < 12; i++)
			{
				if (arr[arrID] < arr[i])
				{
					arrID = i;
				}
			}
			
			ret_tag = arrID + 1;
			return ret_tag;
		}
	}
	else if (read_tag != 255)
	{
		if (pressedFlag)
		{
			pressedFlag = 0;
			if (ret_tag != 0)
			{
				//DEBUG_PRINTLN("Tag read: ", ret_tag);
				*eventFlag = 1;
			}
			else
				*eventFlag = 0;
			
			return ret_tag;
		}
	}
	
	*eventFlag = 0;
	return 0;
}

void ScreenBase::drawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
	FTObj.Begin(FT_RECTS);
	FTObj.ColorRGB(color);
	FTObj.Vertex2ii(x0, y0);
	FTObj.Vertex2ii(x1, y1);
	FTObj.ColorRGB(CLR_WHITE);
}

void ScreenBase::drawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color, uint8_t tag)
{
	FTObj.Begin(FT_RECTS);
	FTObj.ColorRGB(color);
	FTObj.Tag(tag);
	FTObj.Vertex2ii(x0, y0);
	FTObj.Vertex2ii(x1, y1);
	FTObj.ColorRGB(CLR_WHITE);
	FTObj.Tag(0);
}

void ScreenBase::drawSplitLines(uint8_t number)
{
	int width = SCREEN_WIDTH / number;
	int x;
	for (int i = 1; i <= number; i++)
	{
		x = width * i;
		drawLine(x, x, SPLIT_LINE_START, SCREEN_HEIGHT, CLR_SPLIT_LINE);
	}
}

void ScreenBase::clearScreen()
{
	FTObj.DLStart();
	clear();
	FTObj.DLEnd();
}

void ScreenBase::Calibrate()
{
	FTObj.Write32(REG_TOUCH_TRANSFORM_A, 32673);
	FTObj.Write32(REG_TOUCH_TRANSFORM_B, 1356);
	FTObj.Write32(REG_TOUCH_TRANSFORM_C, -1086636);
	FTObj.Write32(REG_TOUCH_TRANSFORM_D, 988);
	FTObj.Write32(REG_TOUCH_TRANSFORM_E, -19182);
	FTObj.Write32(REG_TOUCH_TRANSFORM_F, 18557760);
	//int32_t a;
	//int32_t b;
	//int32_t c;
	//int32_t d;
	//int32_t e;
	//int32_t f;
	//
	//FTObj.DLStart();
	//FTObj.ClearColorRGB(64,64,64);
	//FTObj.Clear(1,1,1);
	//FTObj.ColorRGB(0xff, 0xff, 0xff);
	//FTObj.Cmd_Text((FT_DISPLAYWIDTH/2), (FT_DISPLAYHEIGHT/2), 28, FT_OPT_CENTER, "Please Tap on the dot");
	//FTObj.Cmd_Calibrate(0);
	//
	//FTObj.Finish();
	//delay(100);
	//a = FTObj.Read32(REG_TOUCH_TRANSFORM_A);
	//b = FTObj.Read32(REG_TOUCH_TRANSFORM_B);
	//c = FTObj.Read32(REG_TOUCH_TRANSFORM_C);
	//d = FTObj.Read32(REG_TOUCH_TRANSFORM_D);
	//e = FTObj.Read32(REG_TOUCH_TRANSFORM_E);
	//f = FTObj.Read32(REG_TOUCH_TRANSFORM_F);
	//
	//SERIAL_ECHO_START;
	//SERIAL_ECHOPGM("a: ");
	//SERIAL_ECHOLN(a);
	//
	//SERIAL_ECHO_START;
	//SERIAL_ECHOPGM("b: ");
	//SERIAL_ECHOLN(b);
	//SERIAL_ECHO_START;
	//SERIAL_ECHOPGM("c: ");
	//SERIAL_ECHOLN(c);
	//SERIAL_ECHO_START;
	//SERIAL_ECHOPGM("d: ");
	//SERIAL_ECHOLN(d);
	//SERIAL_ECHO_START;
	//SERIAL_ECHOPGM("e: ");
	//SERIAL_ECHOLN(e);
	//SERIAL_ECHO_START;
	//SERIAL_ECHOPGM("f: ");
	//SERIAL_ECHOLN(f);
}

uint32_t ScreenBase::addImageInMemory(const uint8_t* img, uint32_t dataSize, uint32_t inflated, Bitmap_header_t* imgHeader, uint32_t* ram_ptr)
{
	inflateImage(img, dataSize, imgHeader, *ram_ptr);
	uint32_t ptr = *ram_ptr;
	*ram_ptr += dataSize + inflated;
	return ptr;
}

FT800IMPL_SPI ScreenBase::FTObj(FT_CS_PIN, FT_PDN_PIN, FT_INT_PIN);

Bitmap_header_t ScreenBase::Bitmap_headers[] = 
{
	/* format, width, height, stride, array offset */
	// Splash
	{FT_ARGB1555, 429, 60, 429 * 2, 0},
	
	// Home Header
	{FT_ARGB1555, 188, 72, 188 * 2, 0},
	
	// Home screen
	{FT_ARGB1555, 74, 92, 74 * 2, 0},
	
	// Print header		
	{FT_ARGB1555, 117, 72, 117 * 2, 0},
	
	// Arrows
	{FT_ARGB1555, 43, 28, 43 * 2, 0},
		
	// Back Home
	{FT_ARGB1555, 84, 72, 84 * 2, 0},
		
	// Back SD Card
	{FT_ARGB1555, 111, 72, 111 * 2, 0},
		
	// Printing
	{FT_ARGB1555, 200, 72, 200 * 2, 0},
		
	// Printing Paused
	{FT_ARGB1555, 304, 72, 304 * 2, 0},
	
	// Print Canceled
	{FT_ARGB1555, 304, 72, 304 * 2, 0},
		
	// Right Print
	{FT_ARGB1555, 89, 71, 89 * 2, 0},
};
