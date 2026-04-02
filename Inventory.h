#pragma once

#include <math.h>
#include "ExtraMaths.h"
#include "Object.h"
#include "Triangle.h"
#include "Enemy.h"
#include <malloc.h>

struct bullet {
    vec3 pos;
    vec3 velocity;
    vec3 bulletSize;

    Object* bullet3D;
    Object* hit3D;

    float drop;
    float damage;
    int mode;

    int modeTimer;

    // The timer of each mode - for example how long it takes for the blown up bullet to disapear

    // 0 - dead
    // 1 - shooting
    // 2 - blown up

    bullet() : pos({ 0,0,0 }), velocity({ 0,0,0 }), mode(0), modeTimer(0), bullet3D(nullptr), hit3D(nullptr), bulletSize({ 0,0,0 }), drop(0), damage(0){
    }

    // Parameterised constructor
    bullet(vec3 posp, vec3 velocityp, Object* bullet3Dp, Object* hit3Dp, vec3 bulletSizep, float dropp, float damagep)
        : pos(posp), velocity(velocityp), hit3D(hit3Dp), bullet3D(bullet3Dp), bulletSize(bulletSizep), drop(dropp), damage(damagep){

        hit3D->movePos(pos);
        bullet3D->movePos(pos);

        mode = 1;
        modeTimer = 0;
    }

    void newBullet(Object& bullet3Dp, Object& hit3Dp, vec3 posp, vec3 velocityp) {
        bullet3D = &bullet3Dp;
        hit3D = &hit3Dp;

        pos = posp;
        velocity = velocityp;

        mode = 0;
        modeTimer = 0;
    }

    void update(Object* objects[], int numObjects, Enemy* enemies[], int currentEnemies) {
        switch (mode) {
            case 0:

                break;
            case 1:
                vec3 tempVelocity = velocity;
                tempVelocity.y -= drop;
                vec3 tempPos = pos;

                collision(tempVelocity, tempPos, objects, numObjects);
                collisionEnemies(tempVelocity, tempPos, enemies, currentEnemies);

                pos = tempPos;
                velocity = tempVelocity;

                if (bullet3D != nullptr) {
                    bullet3D->movePos(pos);
                }
                if (hit3D != nullptr) {
                    hit3D->movePos(pos);
                }

                modeTimer++;
                break;
            case 2:
                // Can be any value, but 10 is a nice time since it gives time for the player to see the impact of their bullets
                if (modeTimer > 10) {
                    mode = 0;
                }
                else {
                    hit3D->scale(1.0f, 1.15f, pos);
                    bullet3D->scale(1.0f, 1.15f, pos);
                    modeTimer++;
                }
                break;
        }
    }

    void collisionEnemies(vec3& tempVelocity, vec3 tempPos, Enemy* enemies[], int currentEnemies) {
        bool finished = false;
        int count = 0;

        vec3 intersectionPoint;
        float nearestDistance = 10000000000.0f;
        bool foundCollisionAtAll = false;

        if (enemies == nullptr) {
            // Handle null pointer case
            return;
        }

        for (int i = 0; i < currentEnemies; i++) {
            enemies[i]->checkBulletCollision(&intersectionPoint, &nearestDistance, &foundCollisionAtAll, tempPos, tempVelocity, bulletSize, damage);
        }

        if (foundCollisionAtAll) {
            // Hits scene
            collisionResponse(intersectionPoint, tempPos, tempVelocity, nearestDistance, bulletSize);
            tempVelocity.x = tempVelocity.x * bulletSize.x;
            tempVelocity.y = tempVelocity.y * bulletSize.y;
            tempVelocity.z = tempVelocity.z * bulletSize.z;

            tempPos.x = tempPos.x * bulletSize.x;
            tempPos.y = tempPos.y * bulletSize.y;
            tempPos.z = tempPos.z * bulletSize.z;

            mode = 2;
            modeTimer = 0;
        }
        else {
            tempPos = tempPos + tempVelocity;
            finished = true;
        }

        count = count + 1;
    }

    void collision(vec3& tempVelocity, vec3& tempPos, Object* objects[], int numObjects) {
        bool finished = false;
        int count = 0;

        vec3 intersectionPoint;
        float nearestDistance = 10000000000.0f;
        bool foundCollisionAtAll = false;

        if (objects == nullptr) {
            // Handle null pointer case
            return;
        }

        for (int i = 0; i < numObjects; i++) {
            objects[i]->checkIfHitsPlane(&intersectionPoint, &nearestDistance, &foundCollisionAtAll, tempPos, tempVelocity, bulletSize);
        }

        if (foundCollisionAtAll) {
            // Hits scene
            collisionResponse(intersectionPoint, tempPos, tempVelocity, nearestDistance, bulletSize);
            tempVelocity.x = tempVelocity.x * bulletSize.x;
            tempVelocity.y = tempVelocity.y * bulletSize.y;
            tempVelocity.z = tempVelocity.z * bulletSize.z;

            tempPos.x = tempPos.x * bulletSize.x;
            tempPos.y = tempPos.y * bulletSize.y;
            tempPos.z = tempPos.z * bulletSize.z;

            mode = 2;
            modeTimer = 0;
        }
        else {
            tempPos = tempPos + tempVelocity;
            finished = true;
        }

        count = count + 1;
    }

    void draw(unsigned char*& bits, int frameCount, int bottom, int right, float*& zBuffer, float playerRotationX, float playerRotationY, vec3 playerPos, float FOV) {
        if (mode == 1) {
            // Make sure bullet3D is not nullptr
            if (bullet3D != nullptr) {
                bullet3D->draw(bits, frameCount, bottom, right, zBuffer, playerRotationX, playerRotationY, playerPos, FOV);
            }
        }
        else if (mode == 2) {
            // Make sure hit3D is not nullptr
            if (hit3D != nullptr) {
                hit3D->draw(bits, frameCount, bottom, right, zBuffer, playerRotationX, playerRotationY, playerPos, FOV);
            }
        }
        else {
            // Dead bullet so return from function
            return;
        }
    }
};

struct weapon {
    float damage;
    float bulletSpeed;
    float drop;

    int maxBullets;
    int fireFrames; // How many frames it takes to shoot again

    TEXTURE* icon;
    Object* bullet3D;
    Object* hit3D;

    vec3 bulletSize;

    const char* name;

    // Default constructor
    weapon() : damage(0), bulletSpeed(0), maxBullets(0), icon(nullptr), bullet3D(nullptr), hit3D(nullptr), fireFrames(0), name(nullptr), bulletSize({ 0,0,0 }), drop(0){
    }

    // Parameterised constructor
    weapon(float damagep, float bulletSpeedp, bool bouncesp, int maxBulletsp, TEXTURE* iconp, Object* hit3Dp, Object* bullet3Dp, int fireFramesp, const char* namep, vec3 bulletSizep, float dropp)
        : damage(damagep), bulletSpeed(bulletSpeedp), maxBullets(maxBulletsp), icon(iconp), fireFrames(fireFramesp), hit3D(hit3Dp), bullet3D(bullet3Dp), name(namep), bulletSize(bulletSizep), drop(dropp){
    }
};