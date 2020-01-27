#include <stdio.h>
#include <3ds.h>

#define TOP_WIDTH 240
#define TOP_HEIGHT 800

#define BOTTOM_WIDTH 320
#define BOTTOM_HEIGHT 240

void drawPixel(gfxScreen_t screen, int x, int y, u8* buffer, u8 red, u8 green, u8 blue);
void fillBuffer(gfxScreen_t screen, u8* buffer, u8 red, u8 green, u8 blue);
void clearBuffer(gfxScreen_t screen, u8* buffer);

int main(int argc, char* argv[])
{	
	gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, false);
	
	PrintConsole console;
	
	consoleInit(GFX_BOTTOM, &console);
	consoleSelect(&console);
	
	int x = 0;
	int loops = 0;
	
	bool right = true;
	
	while (aptMainLoop())
	{
		hidScanInput();
		
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) 
			break;
		
		if(x < 0) {
			right = true;
		} else if(x < TOP_WIDTH) {
			right = false;
		}
		
		if(loops % 100 == 0) {
			x = right ? x+1 : x-1;
		}
		
		consoleClear();
		printf("\x1b[0;17Hx=%d", x);*/
		
		//drawPixel(GFX_TOP, x, 400/2, gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0), 0xFF, 0xFF, 0xFF);
		fillBuffer(GFX_TOP, gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0), 0x8A, 0xDA, 0xDA);
				
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}

void drawPixel(gfxScreen_t screen, int x, int y, u8* buffer, u8 red, u8 green, u8 blue) {
	int bufferLocation;
	if(screen == GFX_TOP) {
		bufferLocation = (x * TOP_WIDTH + y) * 3;
	} else {
		bufferLocation = x * BOTTOM_WIDTH + y;
	}
	
	buffer[bufferLocation] = blue;
	buffer[++bufferLocation] = green;
	buffer[++bufferLocation] = red;
}

void fillBuffer(gfxScreen_t screen, u8* buffer, u8 red, u8 green, u8 blue) {
	int bufferSize;
	
	if(screen == GFX_TOP) {
		bufferSize = TOP_WIDTH * TOP_HEIGHT * 3; // 3 subpixels per pixel
	} else {
		bufferSize = BOTTOM_WIDTH * BOTTOM_HEIGHT * 3; // see above
	}
	
	for(int i = 0; i < bufferSize; i++) {
		buffer[i] = blue;
		buffer[++i] = green;
		buffer[++i] = red;
	}
}

void clearBuffer(gfxScreen_t screen, u8* buffer) {
	int bufferLength;
	
	if(screen == GFX_TOP) {
		bufferLength = TOP_WIDTH * TOP_HEIGHT * 3; // see function above
	} else {
		bufferLength = BOTTOM_WIDTH * BOTTOM_HEIGHT * 3; // ^
	}
	
	for(int i = 0; i < bufferLength; i++) {
		buffer[i] = 0x00;
	}
}