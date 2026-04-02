#pragma once

#include <math.h>
#include "ExtraMaths.h"
#include "Triangle.h"
#include <malloc.h>

//object struct
struct Object {
    Triangle* triangles;  // Pointer to an array of Triangle instances
    int numTriangles;     // Number of triangles in the object

    //These are self explanatory
    vec3 pos;
    float facingX;
    float facingY;
    float scaleVal;

    // Constructor to initialize the struct
    Object(int numTriangles, float initPosX, float initPosY, float initPosZ, float initFacingX, float initFacingY, float initScale) {
        this->numTriangles = numTriangles;
        triangles = static_cast<Triangle*>(malloc(numTriangles * sizeof(Triangle))); // Allocate memory for the triangles
        
        facingX = 0;
        facingY = 0;
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;

        vec3 initPos;

        initPos.x = initPosX;
        initPos.y = initPosY;
        initPos.z = initPosY;

        this->scale(1.0f, initScale, pos);
        this->movePos(initPos);
        this->moveFacingX(pos, facingX, initFacingX);
        this->moveFacingY(pos, facingY, initFacingY, facingX);
        
        facingX = initFacingX;
        facingY = initFacingY;
        pos = initPos;
    }

    // Destructor to clean up allocated memory
    ~Object() {
        free(triangles); // Deallocate memory when the object is destroyed
    }

    // Copy constructor
    Object(const Object& other) : numTriangles(other.numTriangles), facingX(other.facingX), facingY(other.facingY), scaleVal(other.scaleVal), pos(other.pos) {
        // Allocate memory for the triangles
        triangles = static_cast<Triangle*>(malloc(numTriangles * sizeof(Triangle)));

        // Copy the triangles from the source object to the new object
        for (int i = 0; i < numTriangles; i++) {
            triangles[i] = other.triangles[i];
        }
    }

    int getNumTriangles() {
        return numTriangles;
    }

    void scale(float oldScale, float currentScale, vec3 scaleCentre) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].scale(oldScale, currentScale, scaleCentre);
        }

        scaleVal = currentScale;
    }

    void movePos(vec3 newPos) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].translate(pos, newPos);
        }

        pos = newPos;
    }

    void moveFacingX(vec3 rotationCentre, float oldFacingX, float newFacingX) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].rotateX(rotationCentre, oldFacingX, newFacingX);
        }
    }

    void moveFacingY(vec3 rotationCentre, float oldFacingY, float newFacingY, float facingX) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].rotateY(rotationCentre, oldFacingY, newFacingY, facingX);
        }
    }

    //Will maybe need facing z as well...
    void draw(unsigned char*& bits, int frameCount, int bottom, int right, float*& zBuffer, float playerRotationX, float playerRotationY, vec3 playerPos, float FOV) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].Draw(bits, frameCount, bottom, right, zBuffer, playerRotationX, playerRotationY, playerPos, FOV);
        }
    }

    void checkIfHitsPlane(vec3* intersectionPoint, float* nearestDistance, bool* foundCollisionAtAll, vec3 pos, vec3 vel, vec3 playerSize) {
        for (int i = 0; i < numTriangles; i++) {
            triangles[i].checkCollision(intersectionPoint, nearestDistance, foundCollisionAtAll, pos, vel, playerSize);
        }
    }
};