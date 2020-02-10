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
void displayTurn(int turn);
void displayScore(int p1_score, int p2_Score);
void displayWinner(bool p1_win, bool p2_win);
int invert(int turn);
int handleInput(u32 kDown, touchPosition touch);

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

	bool _displayWinner = false;
	
	while (aptMainLoop())
	{
		drawImage(GFX_BOTTOM, 0, 0, BOTTOM_HEIGHT, BOTTOM_WIDTH, menu, gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, 0, 0));
		consoleClear();	
		choice = -1;

		displayTurn(turn);
		displayScore(p1_score, p2_score);
		if(_displayWinner) displayWinner(p1_win, p2_win);

		touchPosition touch;
		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_START) { break; }
		
		if(chose && turn == 0) {
			if(p1_win) {
				p1_score++;
			}
			if(p2_win) {
				p2_score++;
			}
		}
		
		chose = false;
		choice = handleInput(kDown, touch);

		if(choice != -1) {
			chose = true;	
			if(turn == 0) {
				p1 = choice;
			} else {
				p2 = choice;
			}
		}

		if(chose && turn == 1) {
			_displayWinner = true;
			int winner = getWinner(p1, p2);
			tie = false;
			p1_win = false;
			p2_win = false;

			if(winner == -1) tie = true;
			else if(winner == 0) p1_win = true;
			else p2_win = true;
		}

		if(chose) {
			turn = invert(turn);
		}
		
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		gspWaitForVBlank();
	}
	gfxExit();
	return 0;
}

void displayTurn(int turn) {
	if(turn == 0) {
		printf("\x1b[30;20HP1 TURN");
		return;
	}

	printf("\x1b[30;20HP2 TURN");
}

void displayScore(int p1_score, int p2_score) {
	printf("\x1b[0;0H%d - %d", p1_score, p2_score);
}

void displayWinner(bool p1, bool p2) {
	if(p1) {
		printf("\x1b[4;0HP1 WIN");
	} else {
		printf("\x1b[4;0HP2 WIN");
	}
}

int invert(int turn) {
	if(turn == 0) {
		return 1;
	} else {
		return 0;
	}
}

int handleInput(u32 kDown, touchPosition touch) {
	if(touch.px >= 35 && touch.px <= 284 && kDown & KEY_TOUCH) {
		if(touch.py >= 50 && touch.py < 100) { // rock
			return 0;
		} else if(touch.py >= 100 && touch.py < 149) { // paper 
			return 1;
		} else if(touch.py >= 150 && touch.py <= 200) { // scissors
			return 2;
		}
	}
	return -1;
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
