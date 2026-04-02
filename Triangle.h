#pragma once

#include <math.h>
#include "ExtraMaths.h"
#include <iostream>

class Triangle {
public:
	Triangle(float a, float b, float c, float d, float e, float f, float g, float h, float i, float facingIn, float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, TEXTURE& textureIn) {

		// Vertices defined ->

		Vertex[0].x = a;
		Vertex[0].y = b;
		Vertex[0].z = c;

		Vertex[1].x = d;
		Vertex[1].y = e;
		Vertex[1].z = f;

		Vertex[2].x = g;
		Vertex[2].y = h;
		Vertex[2].z = i;

		//basically just 2d vectors 

		tCoords[0].x = tx1;
		tCoords[0].y = ty1;

		tCoords[1].x = tx2;
		tCoords[1].y = ty2;

		tCoords[2].x = tx3;
		tCoords[2].y = ty3;

		texture = &textureIn;

		//decided to make it byRef to not make a copy of a huge array

		//finds the equation of triangle
		facing = facingIn;
		Normal = findNormal(Vertex);

		Normal = Normal * facing;
		NormalDistance = Normal.dot(Vertex[0]);
	}

	// Copy constructor
	Triangle(const Triangle& other) {
		for (int i = 0; i < 3; i++) {
			Vertex[i] = other.Vertex[i];
			tCoords[i] = other.tCoords[i];
		}
		Normal = other.Normal;
		texture = other.texture;
		facing = other.facing;
		NormalDistance = other.NormalDistance;
		PI = other.PI;
	}

	void translate(vec3 oldPos, vec3 newPos) {
		vec3 translation = newPos - oldPos;

		for (int i = 0; i < 3; i++) {
			Vertex[i] = Vertex[i] + translation;
		}

		Normal = findNormal(Vertex);

		Normal = Normal * facing;
		NormalDistance = Normal.dot(Vertex[0]);
	}

	void rotateX(vec3 rotationCentre, float oldRotationX, float newRotationX) {
		float rotationVal = newRotationX - oldRotationX;

		for (int i = 0; i < 3; i++) {
			Vertex[i] = Vertex[i] - rotationCentre;

			//updating to the new rotation
			Vertex[i] = matrixApply(Vertex[i].x, Vertex[i].y, Vertex[i].z,
				cos(rotationVal), 0, sin(rotationVal),
				0, 1, 0,
				-sin(rotationVal), 0, cos(rotationVal));

			Vertex[i] = Vertex[i] + rotationCentre; //adding back on the centre
		}

		Normal = findNormal(Vertex);

		Normal = Normal * facing;
		NormalDistance = Normal.dot(Vertex[0]);
	}

	void scale(float oldScale, float currentScale, vec3 scaleCentre) {
		float scale = currentScale / oldScale;

		for (int i = 0; i < 3; i++) {
			Vertex[i] = Vertex[i] - scaleCentre;

			Vertex[i] = Vertex[i] * scale;

			Vertex[i] = Vertex[i] + scaleCentre; //adding back on the centre
		}
	}

	void rotateY(vec3 rotationCentre, float oldRotationY, float newRotationY, float rotationX) {
		float rotationVal = newRotationY - oldRotationY;

		//because y spins after x, it does the inv of y first then adds the new x, then adds back on the y

		for (int i = 0; i < 3; i++) {
			Vertex[i] = Vertex[i] - rotationCentre;

			// ROTATION IN MINUS X FIRST
			Vertex[i] = matrixApply(Vertex[i].x, Vertex[i].y, Vertex[i].z,
				cos(-rotationX), 0, sin(-rotationX),
				0, 1, 0,
				-sin(-rotationX), 0, cos(-rotationX));

			Vertex[i] = matrixApply(Vertex[i].x, Vertex[i].y, Vertex[i].z,
				1, 0, 0,
				0, cos(rotationVal), -sin(rotationVal),
				0, sin(rotationVal), cos(rotationVal));

			//ADDING BACK ON THE X ROTATION (rotation in y = 0)
			Vertex[i] = matrixApply(Vertex[i].x, Vertex[i].y, Vertex[i].z,
				cos(rotationX), 0, sin(rotationX),
				0, 1, 0,
				-sin(rotationX), 0, cos(rotationX));

			Vertex[i] = Vertex[i] + rotationCentre; //adding back on the centre
		}

		Normal = findNormal(Vertex);

		Normal = Normal * facing;
		NormalDistance = Normal.dot(Vertex[0]);
	}

	void checkCollision(vec3* intersectionPoint, float* nearestDistance, bool* foundCollisionAtAll, vec3 pos, vec3 vel, vec3 playerSize) {
		vec3 r3Vertices[3];

		for (int i = 0; i < 3; i++) {
			r3Vertices[i].x = Vertex[i].x / playerSize.x;
			r3Vertices[i].y = Vertex[i].y / playerSize.y;
			r3Vertices[i].z = Vertex[i].z / playerSize.z;
		}

		vec3 r3Pos;
		vec3 r3Velocity;
		vec3 r3Normal;
		float r3Distance;

		r3Pos.x = pos.x / playerSize.x;
		r3Pos.y = pos.y / playerSize.y;
		r3Pos.z = pos.z / playerSize.z;

		r3Velocity.x = vel.x / playerSize.x;
		r3Velocity.y = vel.y / playerSize.y;
		r3Velocity.z = vel.z / playerSize.z;

		r3Normal = findNormal(r3Vertices);

		r3Normal = r3Normal * facing;

		r3Distance = -(r3Normal.dot(r3Vertices[0]));

		vec3 normalisedR3Velocity = r3Velocity;
		normalisedR3Velocity.normalize();

		if (calcAngle(r3Normal, normalisedR3Velocity) >= PI / 2) {
			bool embeddedInPlane = false;

			float normalDotVelocity = r3Velocity.dot(r3Normal);

			float signedDistToTrianglePlane = r3Pos.dot(r3Normal) + r3Distance;

			float t0, t1;

			if (normalDotVelocity == 0) {
				if (fabs(signedDistToTrianglePlane >= 1)) {
					return;
				}
				else {
					embeddedInPlane = true;
					t0 = 0;
					t1 = 1;
				}
			}
			else {
				t0 = (1.0f - signedDistToTrianglePlane) / normalDotVelocity;
				t1 = (-1.0f - signedDistToTrianglePlane) / normalDotVelocity;

				// Swap so t0 < t1
				if (t0 > t1) {
					float temp = t1;
					t1 = t0;
					t0 = temp;
				}

				// Check that at least one result is within range:
				if (t0 > 1 || t1 < 0) {
					return;
				}

				// Clamp to [0,1]
				if (t0 < 0.0f) t0 = 0.0f;
				if (t1 < 0.0f) t1 = 0.0f;
				if (t0 > 1.0f) t0 = 1.0f;
				if (t1 > 1.0f) t1 = 1.0f;
			}

			vec3 collisionPoint;
			bool foundCollision = false;
			float t = 1;

			if (!embeddedInPlane) {
				vec3 planeIntersectionPoint = r3Pos - r3Normal + r3Velocity * t0;

				if (checkPointIn3D(r3Vertices, planeIntersectionPoint)) {
					foundCollision = true;
					t = t0;
					collisionPoint = planeIntersectionPoint;
				}
			}

			if (foundCollision == false) {
				float a, b, c;

				float newT;

				float velocitySquaredLength = r3Velocity.dot(r3Velocity);

				a = velocitySquaredLength;

				vec3 baseMinusP, pMinusBase;

				for (int i = 0; i < 3; i++) {
					baseMinusP = r3Pos - r3Vertices[i];
					pMinusBase = r3Vertices[i] - r3Pos;

					b = 2 * r3Velocity.dot(baseMinusP);
					c = pMinusBase.dot(pMinusBase) - 1;

					if (getLowestRoot(a, b, c, t, &newT)) {
						t = newT;
						foundCollision = true;
						collisionPoint = r3Vertices[i];
					}
				}

				for (int i = 0; i < 3; i++) {
					vec3 edge = r3Vertices[(i + 1) % 3] - r3Vertices[i];
					vec3 baseToVertex = r3Vertices[i] - r3Pos;

					float edgeSquaredLength = edge.dot(edge);
					float edgeDotVelocity = edge.dot(r3Velocity);
					float edgeDotBaseToVertex = edge.dot(baseToVertex);

					a = edgeSquaredLength * -velocitySquaredLength + edgeDotVelocity * edgeDotVelocity;
					b = edgeSquaredLength * (2 * r3Velocity.dot(baseToVertex)) - 2 * edgeDotVelocity * edgeDotBaseToVertex;
					c = edgeSquaredLength * (1 - baseToVertex.dot(baseToVertex)) + edgeDotBaseToVertex * edgeDotBaseToVertex;

					if (getLowestRoot(a, b, c, t, &newT)) {
						float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) / edgeSquaredLength;

						if (f >= 0 && f <= 1) {
							t = newT;
							foundCollision = true;
							collisionPoint = r3Vertices[i] + edge * f;
						}
					}
				}
			}

			if (foundCollision == true) {
				float distToCollision = t * r3Velocity.len();

				if (*foundCollisionAtAll == false || distToCollision < *nearestDistance) {
					*nearestDistance = distToCollision;
					*intersectionPoint = collisionPoint;
					*foundCollisionAtAll = true;
				}
			}
		}
	}

	void Draw(unsigned char*& bits, int frameCount, int bottom, int right, float*& zBuffer, float playerRotationX, float playerRotationY, vec3 playerPos, float FOV) {
		//frameCount may not be needed but I choosed to include it anyway
		//d is the near plane 
		/*
		for (int y = 0; y < 200; y++) {
			for (int x = 0; x < 200; x++) {
				size_t pixel_start = (x + (y * 200)) * 4;
				//Blue - Green - Red - Alpha
				bits[pixel_start] = (char)255;
				bits[pixel_start + 1] = 0;
				bits[pixel_start + 2] = 0;
				bits[pixel_start + 3] = 255;
			}
		}
		*/

		float d = 1; //near plane
		float maxDistance = 500; // max distance until the triangles turn black

		//normalising for use in angle calculations
		vec3 normalizedVertex0 = Vertex[0] - playerPos;
		vec3 normalizedVertex1 = Normal;

		normalizedVertex0.normalize();
		normalizedVertex1.normalize();

		//yep an angle
		double angle = calcAngle(normalizedVertex0, normalizedVertex1);

		if (angle < (PI / 2)) {
			return;
		}

		vec3 transformedVertex[3];

		//I still have no idea if this is the best way to do this but it worked in p5.js so im going to do the same

		vec3 pointShownOne; 
		vec3 pointShownTwo;

		vec3 pointBehindOne;
		vec3 pointBehindTwo;

		vec3 tCoordShownOne;
		vec3 tCoordShownTwo;

		vec3 tCoordBehindOne;
		vec3 tCoordBehindTwo;

		int numBehindPlane = 0;

		for (int i = 0; i < 3; i++) {
			//rotations and fun stuff inside of extra maths
			transformedVertex[i] = calculateVertexInCamera(Vertex[i], playerRotationX, playerRotationY, playerPos);
		}

		//probably a better way to do this but oh well it works 
		//it basically finds which coords are behind to work out if mini triangles need to be created
		for (int i = 0; i < 3; i++) {
			if (transformedVertex[i].z < d) {
				numBehindPlane++;
				if (numBehindPlane == 2) {
					pointBehindTwo = transformedVertex[i];
					tCoordBehindTwo = tCoords[i];
					if (pointShownOne.x == transformedVertex[i].x && pointShownOne.y == transformedVertex[i].y && pointShownOne.z == transformedVertex[i].z) {
						pointShownOne = pointShownTwo;
						tCoordShownOne = tCoordShownTwo;
					}
				}
				else {
					pointBehindOne = transformedVertex[i];
					tCoordBehindOne = tCoords[i];
					pointShownOne = transformedVertex[(i + 1) % 3];
					tCoordShownOne = tCoords[(i + 1) % 3];
					pointShownTwo = transformedVertex[(i + 2) % 3];
					tCoordShownTwo = tCoords[(i + 2) % 3];
				}
			}
		}

		if (numBehindPlane == 0) {
			//print normally
			this->printTriangle(bits, frameCount, bottom, right, transformedVertex, angle, maxDistance, FOV, zBuffer, tCoords);
		}
		else if (numBehindPlane == 1) {
			//1 behind
			//triangle one
			float thetaOne = (d - pointBehindOne.z) / (pointShownOne.z - pointBehindOne.z);

			vec3 pointOnLineOne;
			pointOnLineOne.x = pointBehindOne.x + thetaOne * (pointShownOne.x - pointBehindOne.x);
			pointOnLineOne.y = pointBehindOne.y + thetaOne * (pointShownOne.y - pointBehindOne.y);
			pointOnLineOne.z = d;

			//triangle two
			float thetaTwo = (d - pointBehindOne.z) / (pointShownTwo.z - pointBehindOne.z);

			vec3 pointOnLineTwo;
			pointOnLineTwo.x = pointBehindOne.x + thetaTwo * (pointShownTwo.x - pointBehindOne.x);
			pointOnLineTwo.y = pointBehindOne.y + thetaTwo * (pointShownTwo.y - pointBehindOne.y);
			pointOnLineTwo.z = d;

			vec3 coords[3];
			vec3 textures[3];

			coords[0] = pointShownOne;
			coords[1] = pointOnLineOne;
			coords[2] = pointOnLineTwo;

			textures[0] = tCoordShownOne;

			textures[1].x = tCoordBehindOne.x + thetaOne * (tCoordShownOne.x - tCoordBehindOne.x);
			textures[1].y = tCoordBehindOne.y + thetaOne * (tCoordShownOne.y - tCoordBehindOne.y);
			textures[1].z = 0;

			textures[2].x = tCoordBehindOne.x + thetaTwo * (tCoordShownTwo.x - tCoordBehindOne.x);
			textures[2].y = tCoordBehindOne.y + thetaTwo * (tCoordShownTwo.y - tCoordBehindOne.y);
			textures[2].z = 0;

			this->printTriangle(bits, frameCount, bottom, right, coords, angle, maxDistance, FOV, zBuffer, textures);

			coords[1] = pointShownTwo;
			textures[1] = tCoordShownTwo;

			this->printTriangle(bits, frameCount, bottom, right, coords, angle, maxDistance, FOV, zBuffer, textures);
		}
		else if (numBehindPlane == 2) {
			//2 behind
			float thetaOne = (d - pointShownOne.z) / (pointBehindOne.z - pointShownOne.z);

			vec3 pointOnLineOne;
			pointOnLineOne.x = pointShownOne.x + thetaOne * (pointBehindOne.x - pointShownOne.x);
			pointOnLineOne.y = pointShownOne.y + thetaOne * (pointBehindOne.y - pointShownOne.y);
			pointOnLineOne.z = d;

			float thetaTwo = (d - pointShownOne.z) / (pointBehindTwo.z - pointShownOne.z);

			vec3 pointOnLineTwo;
			pointOnLineTwo.x = pointShownOne.x + thetaTwo * (pointBehindTwo.x - pointShownOne.x);
			pointOnLineTwo.y = pointShownOne.y + thetaTwo * (pointBehindTwo.y - pointShownOne.y);
			pointOnLineTwo.z = d;

			vec3 coords[3];
			vec3 textures[3];

			coords[0] = pointShownOne;
			coords[1] = pointOnLineOne;
			coords[2] = pointOnLineTwo;

			textures[0] = tCoordShownOne;

			textures[1].x = tCoordShownOne.x + thetaOne * (tCoordBehindOne.x - tCoordShownOne.x);
			textures[1].y = tCoordShownOne.y + thetaOne * (tCoordBehindOne.y - tCoordShownOne.y);
			textures[1].z = 0;

			textures[2].x = tCoordShownOne.x + thetaTwo * (tCoordBehindTwo.x - tCoordShownOne.x);
			textures[2].y = tCoordShownOne.y + thetaTwo * (tCoordBehindTwo.y - tCoordShownOne.y);
			textures[2].z = 0;

			this->printTriangle(bits, frameCount, bottom, right, coords, angle, maxDistance, FOV, zBuffer, textures);
		}
		else {
			return;
		}
	}

private:
	vec3 Vertex[3];
	vec3 tCoords[3];
	vec3 Normal;

	TEXTURE* texture;

	float facing;

	float NormalDistance;
	float PI = 3.141592653589793238462643383279502884197f; //yep just the value of pi (I think...)

	void printTriangle(unsigned char*& bits, int frameCount, int bottom, int right, vec3 transformedVertex[], double angle, float maxDistance, float FOV, float*& zBuffer, vec3 tCoords[3]) {
		//makes sure that it doesnt try to draw behond the screen
		int minY = bottom;
		int minX = right;
		int maxY = 0;
		int maxX = 0;

		for (int i = 0; i < 3; i++) {
			if (((transformedVertex[i].x * FOV) / transformedVertex[i].z) + (right / 2) < minX) {
				minX = ceil(((transformedVertex[i].x * FOV) / transformedVertex[i].z) + (right / 2));
			}
			if (((transformedVertex[i].x * FOV) / transformedVertex[i].z) + (right / 2) > maxX) {
				//this is ceil basically
				maxX = floor(((transformedVertex[i].x * FOV) / transformedVertex[i].z) + (right / 2) + 1);
			}
			if ((-(transformedVertex[i].y * FOV) / transformedVertex[i].z) + (bottom / 2) < minY) {
				minY = ceil((-(transformedVertex[i].y * FOV) / transformedVertex[i].z) + (bottom / 2));
			}
			if ((-(transformedVertex[i].y * FOV) / transformedVertex[i].z) + (bottom / 2) > maxY) {
				//this is ceil basically
				maxY = floor((-(transformedVertex[i].y * FOV) / transformedVertex[i].z) + (bottom / 2) + 1);
			}
		}

		if (minX < 0) {
			minX = 0;
		}
		if (maxX > right) {
			maxX = right;
		}
		if (minY < 0) {
			minY = 0;
		}
		if (maxY > bottom) {
			maxY = bottom;
		}

		vec3 distNormal = findNormal(transformedVertex);
		float distNormalDistance = distNormal.dot(transformedVertex[0]);

		// precompute stuff
		float triangleArea = triArea(transformedVertex[0].x, transformedVertex[0].y, transformedVertex[1].x, transformedVertex[1].y, transformedVertex[2].x, transformedVertex[2].y);
		float invTriangleArea = 1 / triangleArea; //inverse of trinagle area thought it might be quicker than division
		float denom = (transformedVertex[1].y - transformedVertex[2].y) * (transformedVertex[0].x - transformedVertex[2].x) + (transformedVertex[2].x - transformedVertex[1].x) * (transformedVertex[0].y - transformedVertex[2].y);

		float blueValue, greenValue, redValue, alphaValue, distance, greyness;
		int tX, tY, idx;

		for (int y = minY; y < maxY; y++) {
			for (int x = minX; x < maxX; x++) {

				distance = (distNormalDistance * FOV) / (distNormal.x * (x - (right / 2)) + distNormal.y * (-y + (bottom / 2)) + distNormal.z * FOV); //it might be distNormal... * FOV...

				if (distance < zBuffer[right * y + x] && pointInsideTriangle(distance * (x - (right / 2)) / (FOV), distance * (-y + (bottom / 2)) / (FOV), transformedVertex[0].x, transformedVertex[0].y, transformedVertex[1].x, transformedVertex[1].y, transformedVertex[2].x, transformedVertex[2].y, denom)) {
					//Coords inside of triangle converted into the same spot inside of the texture coords
					//
					barycentric(distance * (x - (right / 2)) / (FOV), distance * (-y + (bottom / 2)) / (FOV), transformedVertex[0].x, transformedVertex[0].y, transformedVertex[1].x, transformedVertex[1].y, transformedVertex[2].x, transformedVertex[2].y, tCoords[0].x, tCoords[0].y, tCoords[1].x, tCoords[1].y, tCoords[2].x, tCoords[2].y, invTriangleArea, tX, tY);

					// Get the pixel colour using direct pixel array access
					if (!((tX >= 0 && tY < texture->width) && (tY >= 0 && tY < texture->height))) {
						continue;
					}

					idx = 4 * (tX + tY * texture->width);
					blueValue = static_cast<float>(texture->pixels[idx]);
					greenValue = static_cast<float>(texture->pixels[idx + 1]);
					redValue = static_cast<float>(texture->pixels[idx + 2]);
					alphaValue = static_cast<float>(texture->pixels[idx + 3]);

					if (blueValue == 192 && greenValue == 128 && redValue == 255 && alphaValue == 255) {
						continue; // Skip this pixel
					}

					greyness = (1 - angle / (PI)) * (distance / maxDistance) * 255.0f;

					if (greyness > 255.0f) {
						greyness = 255.0f;
					}

					greyness = 255 - greyness;

					int pixel_start = (x + (y * right)) * 4;

					//Blue - Green - Red - Alpha

					bits[pixel_start] = static_cast<int>((greyness * blueValue) / 255.0f);
					bits[pixel_start + 1] = static_cast<int>((greyness * greenValue) / 255.0f);
					bits[pixel_start + 2] = static_cast<int>((greyness * redValue) / 255.0f);
					bits[pixel_start + 3] = alphaValue;

					zBuffer[right * y + x] = distance;
				}
			}
		}
	}
};