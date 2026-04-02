#include <math.h>
#include "ExtraMaths.h"
#include <iostream>
#include "object.h"

class Enemy {
public:
	Enemy(float px, float py, float pz, float vx, float vy, float vz, float esx, float esy, float esz, float pSpeed, float pSTime, Object& pTriangleArr, float HPp, float maxHPp) {
		pos.x = px;
		pos.y = py;
		pos.z = pz;

		velocity.x = vx;
		velocity.y = vy;
		velocity.z = vz;

		otherVelocity.x = 0.0f;
		otherVelocity.y = 0.0f;
		otherVelocity.z = 0.0f;

		HP = HPp;
		maxHP = maxHPp;

		facingY = 0.0f;
		facingX = 0.0f;

		enemySize.x = esx;
		enemySize.y = esy;
		enemySize.z = esz;

		speed = pSpeed;
		sTime = pSTime;

		triangleArr = new Object(pTriangleArr);
	}

	~Enemy() {
		delete triangleArr;
	}

	void draw(unsigned char*& bits, int frameCount, int bottom, int right, float*& zBuffer, float playerRotationX, float playerRotationY, vec3 playerPos) {
		triangleArr->draw(bits, frameCount, bottom, right, zBuffer, playerRotationX, playerRotationY, playerPos);
	}

	void update(Object* objects[], int numObjects, vec3 playerPos) {
		touchingGround = false;

		velocity.x = 0.0f;
		velocity.y = 0.0f;
		velocity.z = 0.0f;

		otherVelocity.y = otherVelocity.y - gravity;

		vec3 tempOtherVel = otherVelocity;
		vec3 tempVelocity = velocity;
		vec3 tempPos = pos;

		vec3 moveDirection;
		moveDirection.x = playerPos.x - pos.x;
		moveDirection.y = playerPos.y - pos.y;
		moveDirection.z = playerPos.z - pos.z;

		moveDirection.normalize();

		moveDirection = moveDirection * speed;

		tempVelocity = tempVelocity + moveDirection;

		//angle clamping

		if (facingY > PI / 2) {
			facingY = PI / 2;
		}
		else if (facingY < -PI / 2) {
			facingY = -PI / 2;
		}

		if (facingX > PI) {
			facingX = -PI;
		}
		if (facingX < -PI) {
			facingX = PI;
		}

		this->collision(tempVelocity, tempPos, 0.0f, objects, numObjects);
		this->collision(tempOtherVel, tempPos, 0.2f, objects, numObjects);

		pos = tempPos;
		otherVelocity = tempOtherVel;

		triangleArr->movePos(pos);

		triangleArr->moveFacingX(pos, facingX, facingX + 0.005f);

		facingX = facingX + 0.005f;

		triangleArr->moveFacingY(pos, facingY, facingY + 0.005f, facingX);

		facingY = facingY + 0.005f;
	}

	void collision(vec3& tempVelocity, vec3& tempPos, float friction, Object* objects[], int numObjects) {
		bool finished = false;
		int count = 0;

		while (!finished) {

			vec3 intersectionPoint;
			float nearestDistance = 10000000000.0f;
			bool foundCollisionAtAll = false;

			if (objects == nullptr) {
				// Handle null pointer case
				return;
			}

			for (int i = 0; i < numObjects; i++) {
				objects[i]->checkIfHitsPlane(&intersectionPoint, &nearestDistance, &foundCollisionAtAll, tempPos, tempVelocity, enemySize);
			}

			if (foundCollisionAtAll) {
				collisionResponse(intersectionPoint, tempPos, tempVelocity, nearestDistance, enemySize);
				tempVelocity.x = tempVelocity.x * enemySize.x;
				tempVelocity.y = tempVelocity.y * enemySize.y;
				tempVelocity.z = tempVelocity.z * enemySize.z;

				tempPos.x = tempPos.x * enemySize.x;
				tempPos.y = tempPos.y * enemySize.y;
				tempPos.z = tempPos.z * enemySize.z;

				touchingGround = true;
			}
			else {
				tempPos = tempPos + tempVelocity;
				finished = true;
			}


			if (tempVelocity.len() < friction || count > 5 || tempVelocity.len() == 0.0f) {
				tempVelocity.x = 0.0f;
				tempVelocity.y = 0.0f;
				tempVelocity.z = 0.0f;
				finished = true;
			}
			else if (touchingGround) {
				float newLength = tempVelocity.len() - friction;
				tempVelocity.normalize();
				tempVelocity = tempVelocity * newLength;
			}
			count = count + 1;
		}
	}

	int getState() {
		return mode;
	}

	void checkBulletCollision(vec3* intersectionPoint, float* nearestDistance, bool* foundCollisionAtAll, vec3 bPos, vec3 bVel, vec3 bulletSize) {
		triangleArr->checkIfHitsPlane(intersectionPoint, nearestDistance, foundCollisionAtAll, bPos, bVel, bulletSize);
	}

	private:
		vec3 pos;
		vec3 velocity;
		vec3 otherVelocity;
		vec3 enemySize;

		Object* triangleArr;

		float HP;
		float maxHP;
		
		int mode = 1; 
		// 0 = dead
		// 1 = alive

		int timer = 0;
		int sTime = 60;
		float gravity = 1;
		bool touchingGround = false;
		float PI = 3.141592653589793238462643383279502884197f; //yep just the value of pi (I think...)
		float speed = 5.0f;
		float facingX;
		float facingY;
};