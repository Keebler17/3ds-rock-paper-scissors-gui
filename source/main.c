#include <stdio.h>
#include <stddef.h>
#include <3ds.h>
#include "include/game_assets.h"

#define TOP_WIDTH 800
#define TOP_HEIGHT 240

#define BOTTOM_WIDTH 320
#define BOTTOM_HEIGHT 240

void drawPixel(gfxScreen_t screen, int x, int y, u8* buffer, u8 red, u8 green, u8 blue);
void fillBuffer(gfxScreen_t screen, u8* buffer, u8 red, u8 green, u8 blue);
void clearBuffer(gfxScreen_t screen, u8* buffer);
void drawImage(gfxScreen_t screen, int _x, int _y, int height, int width, int image[], u8* buffer);
int getXYPos(int x, int y, gfxScreen_t screen);

int main(int argc, char* argv[])
{
	gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, false);
	
	while (aptMainLoop())
	{
		hidScanInput();
		
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) 
			break;
		
		drawImage(GFX_BOTTOM, 50, 50, 50, 50, menu, gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, 0, 0));
		drawImage(GFX_TOP, 50, 50, 50, 50, menu, gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0));
		
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
		bufferLocation = (x * TOP_HEIGHT + y) * 3;
	} else {
		bufferLocation = (x * BOTTOM_HEIGHT + y) * 3;
	}
	
	buffer[bufferLocation] = blue;
	buffer[bufferLocation+1] = green;
	buffer[bufferLocation+2] = red;
}

void fillBuffer(gfxScreen_t screen, u8* buffer, u8 red, u8 green, u8 blue) {
	int bufferSize;
	
	if(screen == GFX_TOP) {
		bufferSize = (TOP_WIDTH/2) * TOP_HEIGHT * 3; // 3 subpixels per pixel, only one screen side
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
		bufferLength = (TOP_WIDTH/2) * TOP_HEIGHT * 3; // see function above
	} else {
		bufferLength = BOTTOM_WIDTH * BOTTOM_HEIGHT * 3; // ^
	}
	
	for(int i = 0; i < bufferLength; i++) {
		buffer[i] = 0x00;
	}
}

void drawImage(gfxScreen_t screen, int _x, int _y, int height, int width, int image[], u8* buffer) {
	int size = 0;
	
	int index = 0;
	while(image[index] != 2147483647) {
		size++;
		index++;
	}
		
	int yHeight = 0;
	
	if(screen == GFX_TOP) {
		yHeight = TOP_HEIGHT;
	} else {
		yHeight = BOTTOM_HEIGHT;
	}
	
	int loc = getXYPos(_x, _y, screen);
	int imageCounter = 0;
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			buffer[loc++] = image[imageCounter++];
			buffer[loc++] = image[imageCounter++];
			buffer[loc++] = image[imageCounter++];
		}
		loc -= height * 3;
		loc += yHeight * 3;
	}
}

int getXYPos(int x, int y, gfxScreen_t screen) {
	int height = screen == GFX_TOP ? TOP_HEIGHT : BOTTOM_HEIGHT;
	return (x * 3 * height) + (y * 3);
}