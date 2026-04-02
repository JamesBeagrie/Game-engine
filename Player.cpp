#include <math.h>
#include "ExtraMaths.h"
#include <iostream>
#include "object.h"
#include "inventory.h"
#include "Enemy.h"

class Player {
public:
	Player() {
		//pos of player
		pos.x = 161.06f;
		pos.y = 20.0032f;
		pos.z = 282.7535f;

		//vel of player
		velocity.x = 0.0f;
		velocity.y = 0.0f;
		velocity.z = 0.0f;

		//other velocity like gravity
		otherVelocity.x = 0.0f;
		otherVelocity.y = 0.0f;
		otherVelocity.z = 0.0f;

		//player size
		playerSize.x = 10.0f;
		playerSize.y = 20.0f;
		playerSize.z = 10.0f;

		//facing angle
		facingX = -2.419f;
		facingY = 0.0571239f;

		// Field of view
		FOV = 1000.0f;
		maxFOV = 10000.0f;
		minFOV = 0.1f;

		// Weapon data
		currentWeapon = nullptr; // Index starts at 0
	}
	
	void Draw(unsigned char*& bits, int frameCount, int bottom, int right, float*& zBuffer) {
			for (int i = 0; i < bulletIndex; i++) {
				bulletArr[i].draw(bits, frameCount, bottom, right, zBuffer, facingX, facingY, pos, FOV);
			}
	}

	void update(bool w,bool a,bool s,bool d,bool space, bool shift, bool one, bool two, bool rightClick, bool r, Object* objects[], int numObjects, int deltaX, int deltaY, Enemy* enemies[], int currentEnemies) {
		HP = HP + 2.0f;

		bool ableToShoot = bulletTimer > currentWeapon->fireFrames;

		if (rightClick && ableToShoot) {
			// Mouse down / shoot
			this->newBullet();
			bulletTimer = 0;
		}

		if (r) {
			// New ammo
			this->newBulletArr();

		}

		if (HP < 0) {
			// really dead
		}
		else if (HP > maxHP) {
			HP = maxHP;
		}

		for (int i = 0; i < bulletIndex; i++) {
			bulletArr[i].update(objects, numObjects, enemies, currentEnemies);
		}
		
		velocity.x = 0.0f;
		velocity.y = 0.0f;
		velocity.z = 0.0f;

		otherVelocity.y = otherVelocity.y - gravity;

		if (touchingGround == true && space) {
			otherVelocity.y += jumpVelocity;
		}

		vec3 tempOtherVel = otherVelocity;
		vec3 tempVelocity = velocity;
		vec3 tempPos = pos;

		vec3 moveDirection;
		moveDirection.x = 0.0f;
		moveDirection.y = 0.0f;
		moveDirection.z = 0.0f;

		//if any movement keys are pressed timer goes up
		if (w || a || s || d) {
			timer = timer + 1.0f;
		}
		else {
			timer = 0;
		}

		if (timer > sTime) {
			timer = sTime;
		}

		if (w == 1) {
			moveDirection.x = moveDirection.x + sin(facingX);
			moveDirection.z = moveDirection.z + cos(facingX);
		}
		if (a == 1) {
			moveDirection.x = moveDirection.x + sin(facingX - (PI / 2));
			moveDirection.z = moveDirection.z + cos(facingX - (PI / 2));
		}
		if (s == 1) {
			moveDirection.x = moveDirection.x + sin(facingX - PI);
			moveDirection.z = moveDirection.z + cos(facingX - PI);
		}
		if (d == 1) {
			moveDirection.x = moveDirection.x + sin(facingX + (PI / 2));
			moveDirection.z = moveDirection.z + cos(facingX + (PI / 2));
		}

		float a1 = (-speed) / (sTime * sTime);
		float b1 = (2 * speed) / (sTime);

		if (moveDirection.len() > 0.00001f) {
			moveDirection.normalize();
		}

		moveDirection = moveDirection * (a1 * (timer * timer) + b1 * timer);

		tempVelocity = tempVelocity + moveDirection;

		// Update facingAngle and angle clamping
		facingY = facingY + deltaY * PI / 200;
		facingX = facingX + deltaX * PI / 200;

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

		bulletTimer++;
		//std::cout << "pos: " << pos.x << "," << pos.y << "," << pos.z << std::endl;
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
				objects[i]->checkIfHitsPlane(&intersectionPoint, &nearestDistance, &foundCollisionAtAll, tempPos, tempVelocity, playerSize);
			}

			if (foundCollisionAtAll) {
				collisionResponse(intersectionPoint, tempPos, tempVelocity, nearestDistance, playerSize);
				tempVelocity.x = tempVelocity.x * playerSize.x;
				tempVelocity.y = tempVelocity.y * playerSize.y;
				tempVelocity.z = tempVelocity.z * playerSize.z;

				tempPos.x = tempPos.x * playerSize.x;
				tempPos.y = tempPos.y * playerSize.y;
				tempPos.z = tempPos.z * playerSize.z;

				touchingGround = true;
			}
			else {
				tempPos = tempPos + tempVelocity;
				finished = true;
				touchingGround = false;
			}


			if (tempVelocity.len() < friction || count > 5 || tempVelocity.len() == 0.0f) {
				tempVelocity.x = 0.0f;
				tempVelocity.y = 0.0f;
				tempVelocity.z = 0.0f;
				finished = true;
			}
			else if (touchingGround){
				float newLength = tempVelocity.len() - friction;
				tempVelocity.normalize();
				tempVelocity = tempVelocity * newLength;
			}
			count = count + 1;
		}
	}

	void changeFacingX(float value) {
		facingX = facingX + value;
	}

	// Sets the weapon pointer to a new value
	void setWeaponPtr(weapon* ptr) {
		currentWeapon = ptr;
		newBulletArr();
	}

	float getFOV() {
		return FOV;
	}

	float getMinFOV() {
		return minFOV;
	}

	float getMaxFOV() {
		return maxFOV;
	}

	float* getFOVpointer() {
		return &FOV;
	}

	// Returns the current index position 
	int getWeaponIndex() {
		return weaponIndex;
	}

	//sets the index position to a new value
	void setWeaponIndex(int weaponIndexp) {
		weaponIndex = weaponIndexp;
	}

	int getBulletIndex() {
		return bulletIndex;
	}

	int getMaxBullets() {
		if (currentWeapon) {
			return currentWeapon->maxBullets;
		}
		else {
			return 0;
		}
	}

	vec3 getPos() {
		return pos;
	}

	vec3 getVelocity() {
		return velocity;
	}

	float getFacingX() {
		return facingX;
	}

	float getFacingY() {
		return facingY;
	}

	float getHP() {
		return HP;
	}

	float getMaxHP() {
		return maxHP;
	}

	void drawWeaponBMP(unsigned char* bits, TEXTURE* font[], int screenWidth, int screenHeight) {
		if (currentWeapon != nullptr) {
			drawBMP(bits, floor(screenWidth * 0.99 - screenHeight * 0.18), floor(screenHeight * 0.84), floor(screenWidth * 0.99), floor(screenHeight * 0.96), currentWeapon->icon, screenWidth, screenHeight);

			textDraw(currentWeapon->name, bits, floor(screenWidth * 0.99 - screenHeight * 0.18), floor(screenHeight * 0.76), floor(screenWidth * 0.99), floor(screenHeight * 0.81), font, screenWidth, screenHeight);
		}
	}

	void newBulletArr() {
		if (bulletArr) {
			free(bulletArr);
		}

		bulletArr = (bullet*)malloc(sizeof(bullet) * currentWeapon->maxBullets);
		bulletIndex = 0;
		bulletTimer = 0;
	}

	void newBullet() {
		if (bulletIndex >= currentWeapon->maxBullets) {
			// Ran out of bullets 
			std::cout << "Out of bullets" << std::endl;
		}
		else {
			// Create new bullet
			std::cout << "Kaboom" << std::endl;

			Object* copiedBullet3D = new Object(*currentWeapon->bullet3D);

			Object* copiedHit3D = new Object(*currentWeapon->hit3D);

			vec3 facingDirection;

			facingDirection.x = cosf(facingY) * sinf(facingX);
			facingDirection.y = -sinf(facingY);
			facingDirection.z = cosf(facingY) * cosf(facingX);

			facingDirection = facingDirection * currentWeapon->bulletSpeed;

			bulletArr[bulletIndex] = bullet(pos, facingDirection, copiedBullet3D, copiedHit3D,currentWeapon->bulletSize, currentWeapon->drop, currentWeapon->damage);
			bulletIndex++;
		}
	}

	void w() {
		pos.x = pos.x + sin(facingX)* speed;
		pos.z = pos.z + cos(facingX)* speed;
	}
	void a() {
		pos.x = pos.x + sin(facingX - (PI / 2)) * speed;
		pos.z = pos.z + cos(facingX - (PI / 2)) * speed;
	}
	void s() {
		pos.x = pos.x + sin(facingX - PI) * speed;
		pos.z = pos.z + cos(facingX - PI) * speed;
	}
	void d() {
		pos.x = pos.x + sin(facingX + (PI / 2)) * speed;
		pos.z = pos.z + cos(facingX + (PI / 2)) * speed;
	}
	void left() {
		facingX = facingX - 0.005f;
	}
	void right() {
		facingX = facingX + 0.005f;
	}
	void up() {
		facingY = facingY + 0.005f;
	}
	void down() {
		facingY = facingY - 0.005f;
	}

private:
	vec3 pos;
	vec3 velocity;
	vec3 otherVelocity;
	vec3 playerSize;
	int timer = 0;
	int sTime = 60;

	float gravity = 1.0f;
	bool touchingGround = false;
	float PI = 3.141592653589793238462643383279502884197f; //yep just the value of pi (I think...)
	float speed = 5.0f;
	float jumpVelocity = 10.0f;
	float facingX;
	float facingY;

	float FOV;
	float maxFOV;
	float minFOV;
	
	float gold = 0.0f;
	float HP = 1000.0f;
	float maxHP = 1000.0f;

	weapon* currentWeapon = nullptr;
	bullet* bulletArr = nullptr;

	int weaponIndex = 0;
	int bulletIndex = 0; // Points to the next array position

	int bulletTimer = 0;
};