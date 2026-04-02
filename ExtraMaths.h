#pragma once

#include <math.h>
#include <malloc.h>
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <cstdio>

struct vec3 {
    float x;
    float y;
    float z;

    vec3 operator+(const vec3& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    vec3 operator+(const float& scalar) const {
        return { x + scalar, y + scalar, z + scalar };
    }

    vec3 operator-(const vec3& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    vec3 operator-() const {
        return { -x, -y, -z };
    }

    vec3 operator*(const float& scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    vec3 operator/(const float& scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }

    float dot(const vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float len() const {
        return sqrtf(this->dot(*this));
    }

    void normalize() {
        float len = this->len();
        x /= len;
        y /= len;
        z /= len;
    }
};

struct TEXTURE {
    // Texture structure type

    // Width and Height of image
    int width = 0;
    int height = 0;

    // Pixel array
    unsigned char* pixels = nullptr;

    // Deallocate memory
    ~TEXTURE() {
        if (pixels != nullptr) {
            free(pixels);
            pixels = nullptr;
        }
    }

    void manualTexture(int widthIN, int heightIN) {
        size_t size = widthIN * heightIN * 4;
        width = widthIN;
        height = heightIN;
        pixels = (unsigned char*)malloc(size);
    }

    void read(const char* path) {

        std::ifstream f;
        f.open(path, std::ios::in | std::ios::binary);

        if (!f.is_open()) {
            std::cout << "File not opened" << std::endl;

            width = 2;
            height = 2;

            size_t pixelSize = width * height * 4;
            pixels = (unsigned char*)malloc(pixelSize);

            pixels[0] = 255;
            pixels[1] = 0;
            pixels[2] = 255;
            pixels[3] = 255; // Pixel one

            pixels[4] = 0;
            pixels[5] = 0;
            pixels[6] = 0;
            pixels[7] = 255; // Pixel two

            pixels[8] = 255;
            pixels[9] = 0;
            pixels[10] = 255;
            pixels[11] = 255; // Pixel three

            pixels[12] = 0;
            pixels[13] = 0;
            pixels[14] = 0; 
            pixels[15] = 255; // Pixel four

            return;
        }

        const int fileHeaderSize = 14;
        const int informationHeaderSize = 40;

        unsigned char fileHeader[fileHeaderSize];
        f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

        unsigned char informationHeader[informationHeaderSize];
        f.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

        int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);

        width = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
        height = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

        size_t pixelSize = width * height * 4;
        pixels = (unsigned char*)malloc(pixelSize);

        const int paddingAmount = ((4 - (width * 3) % 4) % 4);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned char color[3];
                f.read(reinterpret_cast<char*>(color), 3);

                size_t current = (y * width + x) * 4;
                // B G R A
                pixels[current] = color[0];
                pixels[current + 1] = color[1];
                pixels[current + 2] = color[2];
                pixels[current + 3] = 255;
            }
            f.ignore(paddingAmount);
        }
        
        f.close();
    }
};

void drawBMP(unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE* img, int screenWidth, int screenHeight);

void textDraw(char* text, unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE* font[], int screenWidth, int screenHeight);

void textDraw(const char* text, unsigned char* bits, int startX, int startY, int endX, int endY, TEXTURE* font[], int screenWidth, int screenHeight);

bool checkPointIn3D(vec3 vertices[], vec3 planeIntersectionPoint);

double calcAngle(vec3 lineOne, vec3 lineTwo);

vec3 matrixApply(float x, float y, float z, float a, float b, float c, float d, float e, float f, float g, float h, float i);

vec3 findNormal(vec3 points[]);

void collisionResponse(vec3 intersectionPoint, vec3& pos, vec3& vel, float nearestDistance, vec3 playerSize);

vec3 calculateVertexInCamera(vec3 point, float rotationX, float rotationY, vec3 playerPos);

float triArea(float x1, float y1, float x2, float y2, float x3, float y3);

bool getLowestRoot(float a, float b, float c, float t, float* newT);

bool pointInsideTriangle(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3, float denom);

void barycentric(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3, float t1x, float t1y, float t2x, float t2y, float t3x, float t3y, float inverseTotalArea, int& return1, int& return2);