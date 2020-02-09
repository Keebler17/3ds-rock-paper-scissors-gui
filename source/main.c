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
int getWinner(int p1, int p2);

int main(int argc, char* argv[])
{
	
	int choice = -1;
	bool chose = false;
	
	int turn = 0;
	
	int p1 = -1;
	int p2 = -1;
	
	int p1_score = 0;
	int p2_score = 0;
	
	gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, false);
	
	PrintConsole console;
	
	consoleInit(GFX_TOP, &console);
	consoleSelect(&console);
	
	bool tie = false;
	bool p1_win = false;
	bool p2_win = false;
	
	while (aptMainLoop())
	{
		// fillBuffer(GFX_BOTTOM, gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, 0, 0), 0x00, 0x00, 0x00);
		drawImage(GFX_BOTTOM, 0, 0, BOTTOM_HEIGHT, BOTTOM_WIDTH, menu, gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, 0, 0));
		consoleClear();
		
		if(tie) {
			printf("\x1b[0;0Htie");
		} else if(p1_win) {
			printf("\x1b[0;0Hp1");
		} else if(p2_win) {
			printf("\x1b[0;0Hp2");
		}
		
		printf("\x1b[2;0H%d - %d", p1_score, p2_score);
		
		touchPosition touch;
		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_START) { 
			break;
		}
		
		
		if(turn == 0) {
			p1 = choice;
		} else {
			p2 = choice;
			
			int result = getWinner(p1, p2);
			
			tie = false;
			p1_win = false;
			p2_win = false;
			
			if(result == -1) {
				tie = true;
			} else if(result == 0) {
				p1_win = true;
			} else {
				p2_win = true;
			}
			
		}
		
		if(chose && !tie) {
			if(p1_win) {
				p1_score++;
			}
			
			if(p2_win) {
				p2_score++;
			}
		}
		
		chose = false;
		if(touch.px >= 35 && touch.px <= 284 && kDown & KEY_TOUCH) {
			if(touch.py >= 50 && touch.py < 100) { // rock
				choice = 0;
				if(turn == 0) {
					turn = 1;
				} else {
					turn = 0;
				}
				chose = true;
			} else if(touch.py >= 100 && touch.py < 149) { // paper 
				choice = 1;
				if(turn == 0) {
					turn = 1;
				} else {
					turn = 0;
				}
				chose = true;
			} else if(touch.py >= 150 && touch.py <= 200) { // scissors
				choice = 2;
				if(turn == 0) {
					turn = 1;
				} else {
					turn = 0;
				}
				chose = true;
			}
		}
		
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

int getWinner(int p1, int p2) {
	if (p1 == 0) {
		if(p2 == 0) {
			return -1;
		} else if (p2 == 1) {
			return 1;
		} else if (p2 == 2) {
			return 0;
		}
	} else if (p1 == 1) {
		if (p2 == 0) {
			return 0;
		} else if(p2 == 1) {
			return -1;
		} else if (p2 == 2) {
			return 1;
		}
	} else if (p1 == 2) {
		if (p2 == 0) {
			return 1;
		} else if (p2 == 1) {
			return 0;
		} else if(p2 == 2) {
			return -1;
		}
	}
	return 0xFF;
}