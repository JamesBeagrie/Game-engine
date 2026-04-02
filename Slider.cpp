#include <math.h>
#include "ExtraMaths.h"
#include <iostream>

class Slider {
public:
	Slider(int sxp, int syp, int exp, int eyp, TEXTURE& sliderp, TEXTURE& normalp, int modep, const char* namep, int sxtp, int sytp, int extp, int eytp, float* variablep, float maxValuep, float minValuep) {
		sx = sxp;
		sy = syp;
		ex = exp;
		ey = eyp;

		name = namep;

		sxt = sxtp;
		syt = sytp;
		ext = extp;
		eyt = eytp;

		sliderTexture = &sliderp;
		normalTexture = &normalp;

		variable = variablep;
		maxValue = maxValuep;
		minValue = minValuep;


		sliderDistance = ext;
		mode = modep;
	}

	void draw(unsigned char*& bits, int screenWidth, int screenHeight, int mx, int my, bool mousedown, int currentMode, TEXTURE* font[]) {
		if (mode == currentMode) { // Only draw the button if it's not hidden
			sliderDistance = ((*variable - minValue) * ex) / (maxValue - minValue) + ext;
			//checks to see if the mouse coords are inside of the button
			drawBMP(bits, sx, sy, ex, ey, normalTexture, screenWidth, screenHeight);
			textDraw(name, bits, sxt, syt, ext, eyt, font, screenWidth, screenHeight);

			// Check if the mouse is pressed
			if (mousedown && mx > ext && mx < ex && my > sy && my < ey){
				sliderDistance = mx;

				float percentage = (sliderDistance - ext) / ex;

				if (percentage > 0 && percentage < 1) {
					*variable = (maxValue - minValue) * percentage + minValue;
				}
			}
			//Draw slider BMP
			int halfLength = floor((ex - sx) * 0.03);
			drawBMP(bits, sliderDistance - halfLength, sy, sliderDistance + halfLength, ey, sliderTexture, screenWidth, screenHeight);
		}

		//void textDraw(char* text, unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE * font[], int screenWidth, int screenHeight);
	}

private:
	int sx, sy, ex, ey;
	int sxt, syt, ext, eyt;

	float* variable;
	float maxValue;
	float minValue;

	int mode;

	float sliderDistance;

	const char* name;

	TEXTURE* sliderTexture;
	TEXTURE* normalTexture;
};