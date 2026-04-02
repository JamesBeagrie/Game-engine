#include "ExtraMaths.h"

double calcAngle(vec3 lineOne, vec3 lineTwo) {
    //again, they must be in unit vectors

    double tempCalculation = lineOne.dot(lineTwo);
    double angle = acos(tempCalculation);

    return angle;
    //returns as a double
}

vec3 matrixApply(float x, float y, float z, float a, float b, float c, float d, float e, float f, float g, float h, float i) {

    //just does what the function is called
    vec3 temp;
    temp.x = a * x + b * y + c * z;
    temp.y = d * x + e * y + f * z;
    temp.z = g * x + h * y + i * z;

    return temp;
}

vec3 findNormal(vec3 points[]) {
    //finds normal of a triangle

    vec3 lineOne = points[1] - points[0];
    vec3 lineTwo = points[2] - points[0];

    vec3 crossP;
    crossP.x = lineOne.y * lineTwo.z - (lineOne.z * lineTwo.y);
    crossP.y = lineOne.z * lineTwo.x - (lineOne.x * lineTwo.z);
    crossP.z = lineOne.x * lineTwo.y - (lineOne.y * lineTwo.x);

    crossP.normalize();
    
    return crossP;
}

bool checkPointIn3D(vec3 vertices[], vec3 planeIntersectionPoint) {
    float floatingPointError = 0.0001f;

    vec3 AB = vertices[1] - vertices[0];
    vec3 AC = vertices[2] - vertices[0];
    vec3 AP = planeIntersectionPoint - vertices[0];

    float ABdotAB = AB.dot(AB);
    float APdotAB = AB.dot(AP);
    float ACdotAB = AC.dot(AB);
    float APdotAC = AP.dot(AC);
    float ACdotAC = AC.dot(AC);

    float u = (ACdotAC * APdotAB - ACdotAB * APdotAC) / (ABdotAB * ACdotAC - ACdotAB * ACdotAB);
    float v = (ABdotAB * APdotAC - ACdotAB * APdotAB) / (ABdotAB * ACdotAC - ACdotAB * ACdotAB);
    float w = 1 - u - v;

    if (0 < u && u < 1 && 0 < v && v < 1 && 0 < w && w < 1) {
        // Inside the triangle
        return true;
    }
    else {
        return false;
    }
}

void drawBMP(unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE* img, int screenWidth, int screenHeight) {
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            if (x > 0 && y > 0 && x < screenWidth && y < screenHeight) { // Makes sure that it is inside the screen
                // Finds coords in the img
                int imgX, imgY, idx, blueValue, greenValue, redValue, alphaValue, bitsIdx;
                imgX = floor(((x - startX) * img->width) / (endX - startX));
                // Reverse the y-coordinate or subtract it from img's height
                imgY = img->height - 1 - floor(((y - startY) * img->height) / (endY - startY));

                idx = 4 * (imgX + imgY * img->width);
                blueValue = img->pixels[idx];
                greenValue = img->pixels[idx + 1];
                redValue = img->pixels[idx + 2];
                alphaValue = img->pixels[idx + 3];

                if (blueValue == 192 && greenValue == 128 && redValue == 255 && alphaValue == 255) {
                    continue; // Skip this pixel because horrible pink colour (And to make transparent parts of images)
                }

                bitsIdx = 4 * (x + y * screenWidth);
                bits[bitsIdx] = blueValue;
                bits[bitsIdx + 1] = greenValue;
                bits[bitsIdx + 2] = redValue;
                bits[bitsIdx + 3] = alphaValue;
            }
        }
    }
}

void barycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3, float t1x, float t1y, float t2x, float t2y, float t3x, float t3y, float inverseTotalArea, int& return1, int& return2) {
    float u = fabs(x * (y2 - y3) + x2 * (y3 - y) + x3 * (y - y2)) * inverseTotalArea * 0.5f;
    float v = fabs(x1 * (y - y3) + x * (y3 - y1) + x3 * (y1 - y)) * inverseTotalArea * 0.5f;
    float w = fabs(x1 * (y2 - y) + x2 * (y - y1) + x * (y1 - y2)) * inverseTotalArea * 0.5f;

    return1 = static_cast<int>(u * t1x + v * t2x + w * t3x);
    return2 = static_cast<int>(u* t1y + v * t2y + w * t3y);
}

bool getLowestRoot(float a, float b, float c, float maxR, float* root) {
    // Check if a solution exists
    float determinant = b * b - 4.0f * a * c;
    // If determinant is negative it means no solutions.
    if (determinant < 0.0f) return false;
    // calculate the two roots: (if determinant == 0 then
    // x1==x2 but let’s disregard that slight optimization)
    float sqrtD = sqrt(determinant);
    float r1 = (-b - sqrtD) / (2 * a);
    float r2 = (-b + sqrtD) / (2 * a);
    // Sort so x1 <= x2
    if (r1 > r2) {
        float temp = r2;
        r2 = r1;
        r1 = temp;
    }
    // Get lowest root:
    if (r1 > 0 && r1 < maxR) {
        *root = r1;
        return true;
    }
    // It is possible that we want x2 - this can happen
    // if x1 < 0
    if (r2 > 0 && r2 < maxR) {
        *root = r2;
        return true;
    }
    // No (valid) solutions
    return false;
}

void textDraw(char* text, unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE* font[], int screenWidth, int screenHeight) {
    size_t lengthOfText = strlen(text); //length of text

    int lengthOfEachChar = floor((endX - startX) / (lengthOfText));

    for (int i = 0; i < lengthOfText; i++) {
        
        int arrPoint;

        switch (text[i])
        {
        //UPPERCASE LETTERS
        case 'A':
            arrPoint = 0;
            break;
        case 'B':
            arrPoint = 1;
            break;
        case 'C':
            arrPoint = 2;
            break;
        case 'D':
            arrPoint = 3;
            break;
        case 'E':
            arrPoint = 4;
            break;
        case 'F':
            arrPoint = 5;
            break;
        case 'G':
            arrPoint = 6;
            break;
        case 'H':
            arrPoint = 7;
            break;
        case 'I':
            arrPoint = 8;
            break;
        case 'J':
            arrPoint = 9;
            break;
        case 'K':
            arrPoint = 10;
            break;
        case 'L':
            arrPoint = 11;
            break;
        case 'M':
            arrPoint = 12;
            break;
        case 'N':
            arrPoint = 13;
            break;
        case 'O':
            arrPoint = 14;
            break;
        case 'P':
            arrPoint = 15;
            break;
        case 'Q':
            arrPoint = 16;
            break;
        case 'R':
            arrPoint = 17;
            break;
        case 'S':
            arrPoint = 18;
            break;
        case 'T':
            arrPoint = 19;
            break;
        case 'U':
            arrPoint = 20;
            break;
        case 'V':
            arrPoint = 21;
            break;
        case 'W':
            arrPoint = 22;
            break;
        case 'X':
            arrPoint = 23;
            break;
        case 'Y':
            arrPoint = 24;
            break;
        case 'Z':
            arrPoint = 25;
            break;
        case '0':
            arrPoint = 27;
            break;
        case '1':
            arrPoint = 28;
            break;
        case '2':
            arrPoint = 29;
            break;
        case '3':
            arrPoint = 30;
            break;
        case '4':
            arrPoint = 31;
            break;
        case '5':
            arrPoint = 32;
            break;
        case '6':
            arrPoint = 33;
            break;
        case '7':
            arrPoint = 34;
            break;
        case '8':
            arrPoint = 35;
            break;
        case '9':
            arrPoint = 36;
            break;

        default:
            arrPoint = 26;
            break;
        }

        int start = startX + lengthOfEachChar * i;
        int end = start + lengthOfEachChar;

        drawBMP(bits, start, startY, end, endY, font[arrPoint], screenWidth, screenHeight);
    }
}

void textDraw(const char* text, unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE* font[], int screenWidth, int screenHeight) {

    char* nText = const_cast<char*>(text);

    textDraw(nText, bits, startX, startY, endX, endY, font, screenWidth, screenHeight);
}

void collisionResponse(vec3 intersectionPoint, vec3& pos, vec3& vel, float nearestDistance, vec3 playerSize) {
    pos.x = pos.x / playerSize.x;
    pos.y = pos.y / playerSize.y;
    pos.z = pos.z / playerSize.z;

    vel.x = vel.x / playerSize.x;
    vel.y = vel.y / playerSize.y;
    vel.z = vel.z / playerSize.z;

    float veryCloseDistance = 0.0002f;

    vec3 destinationPoint = pos + vel;
    vec3 newBasePoint = pos;

    if (nearestDistance >= veryCloseDistance) {
        vec3 V = vel;
        V.normalize();
        V = V * (nearestDistance - veryCloseDistance);
        newBasePoint = pos + V;

        V.normalize();
        V = V * veryCloseDistance;
        intersectionPoint = intersectionPoint - V;
    }

    vec3 slidePlaneNormal = newBasePoint - intersectionPoint;
    slidePlaneNormal.normalize();

    float slidePlaneDistance = -(newBasePoint.dot(slidePlaneNormal));

    float theta = (slidePlaneDistance + destinationPoint.dot(slidePlaneNormal)) / (slidePlaneNormal.dot(slidePlaneNormal));

    vel = destinationPoint - slidePlaneNormal * theta - newBasePoint;

    pos = newBasePoint;
}

vec3 calculateVertexInCamera(vec3 point, float rotationX, float rotationY, vec3 playerPos) {
    //rotates the points around the player by the players facing coordinates

    vec3 temp1 = point - playerPos;

    temp1 = matrixApply(temp1.x,temp1.y,temp1.z,
                        cos(-rotationX), 0, sin(-rotationX),
                        0,1,0,
                        -sin(-rotationX), 0, cos(-rotationX));

    temp1 = matrixApply(temp1.x, temp1.y, temp1.z, 
                        1,0,0,
                        0, cos(-rotationY), -sin(-rotationY),
                        0, sin(-rotationY), cos(-rotationY));

    return temp1;
}

float triArea(float x1, float y1, float x2, float y2, float x3, float y3) {
    //calculates triangle area
    return fabs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0f);
}

bool pointInsideTriangle(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3, float denom) {
    // Calculate the barycentric coordinates
    float alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denom;
    float beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denom;
    float gamma = 1.0f - alpha - beta;

    // Check if the point is inside the triangle
    return alpha >= 0 && beta >= 0 && gamma >= 0;
}