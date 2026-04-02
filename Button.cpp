#include <math.h>
#include "ExtraMaths.h"
#include <iostream>

class Button {
public:
	Button(int sxp, int syp, int exp, int eyp, TEXTURE& hoverp, TEXTURE& normalp, int modep, int locationp, const char* namep, int sxtp, int sytp, int extp, int eytp) {
		sx = sxp;
		sy = syp;
		ex = exp;
		ey = eyp;

		name = namep;

		sxt = sxtp;
		syt = sytp;
		ext = extp;
		eyt = eytp;

		hoverTexture = &hoverp;
		normalTexture = &normalp;

		mode = modep;

		location = locationp;
	}

	int draw(unsigned char*& bits, int screenWidth, int screenHeight, int mx, int my, bool mousedown, int currentMode, TEXTURE* font[]) {
		int returnValue = currentMode;
		if (mode == currentMode) { // Only draw the button if it's not hidden
			//checks to see if the mouse coords are inside of the button
			if (mx > sx && mx < ex && my > sy && my < ey) {
				drawBMP(bits, sx, sy, ex, ey, hoverTexture, screenWidth, screenHeight);
				if (mousedown) {
					//mouse is down and inside of the button
					returnValue = location;
				}
			}
			else {
				drawBMP(bits, sx, sy, ex, ey, normalTexture, screenWidth, screenHeight);
			}
			textDraw(name, bits, sxt, syt, ext, eyt, font, screenWidth, screenHeight);
		}

		//void textDraw(char* text, unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE * font[], int screenWidth, int screenHeight);
		return returnValue;
	}

private:
	int sx, sy, ex, ey;
	int sxt, syt, ext, eyt;

	const char* name;

	TEXTURE* hoverTexture;
	TEXTURE* normalTexture;

	int mode;
	int location;
};