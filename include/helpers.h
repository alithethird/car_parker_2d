
#include "raylib.h"
#include "stdlib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include <stdio.h>
#include "parking_sensor.h"
#include "car_map.h"
// Image, pixel data stored in CPU memory (RAM)
typedef struct CarImage
{
	Image body;		   // car body Image
	Image leftWheel;   // front left wheel Image
	Image rightWheel;  // front right wheel Image
	int wheelRotation; // front wheel angle (Degrees)
} CarImage;

typedef struct CarTexture
{
	Texture2D body;		  // car body Image
	Texture2D leftWheel;  // front left wheel Image
	Texture2D rightWheel; // front right wheel Image
	int wheelRotation;	  // front wheel angle (Degrees)
	int carRotation;	  // front wheel angle (Degrees)
	Rectangle bodySourceRec;
	Rectangle bodyDestRec;
	Vector2 bodyOrigin;
	Rectangle rightWheelSourceRec;
	Rectangle rightWheelDestRec;
	Vector2 rightWheelOrigin;
	Rectangle leftWheelSourceRec;
	Rectangle leftWheelDestRec;
	Vector2 leftWheelOrigin;

} CarTexture;

typedef struct SensorStatus
{
	bool hit;
	float sensorHit[8];
} SensorStatus;

// function to load car with wheels
CarImage LoadNewCar(const char *carName)
{
	printf(carName);
	// Load a texture from the resources directory
	Image insignia = LoadImage(carName);

	Image wheel = LoadImage("wheel_black.png");
	Image wheelLeft = LoadImage("wheel_lime.png");
	Image wheelRight = LoadImage("wheel_black.png");
	Image car = LoadImage("insignia_empty.png");
	ImageDraw(&car, insignia, (Rectangle){0, 0, insignia.width, insignia.height}, (Rectangle){0, 0, (float)insignia.width, (float)insignia.height}, WHITE);
	int rear_wheel_height = 160;
	int right_wheel_width = 18;
	int left_wheel_width = 368;
	int front_wheel_height = 745;
	ImageDraw(&car, wheel, (Rectangle){0, 0, (float)wheel.width, (float)wheel.height}, (Rectangle){right_wheel_width, rear_wheel_height, wheel.width, wheel.height}, WHITE);
	ImageDraw(&car, wheel, (Rectangle){0, 0, (float)wheel.width, (float)wheel.height}, (Rectangle){left_wheel_width, rear_wheel_height, wheel.width, wheel.height}, WHITE);
	// ImageDraw(&car, wheel, (Rectangle){ 0, 0, (float)wheel.width, (float)wheel.height }, (Rectangle){ right_wheel_width, front_wheel_height, wheel.width, wheel.height }, WHITE);
	// ImageDraw(&car, wheel, (Rectangle){ 0, 0, (float)wheel.width, (float)wheel.height }, (Rectangle){ left_wheel_width, front_wheel_height, wheel.width, wheel.height }, WHITE);
	ImageDraw(&car, insignia, (Rectangle){0, 0, insignia.width, insignia.height}, (Rectangle){0, 0, (float)insignia.width, (float)insignia.height}, WHITE);
	UnloadImage(insignia);
	// UnloadImage(wheel);
	ImageRotate(&car, 180);
	ImageRotate(&wheelLeft, 180);
	// ImageRotate(&wheel, 180);
	ImageRotate(&wheelRight, 180);
	CarImage newCarImage = {.body = car, .leftWheel = wheelLeft, .rightWheel = wheelRight, .wheelRotation = 0};
	return newCarImage;
}

CarTexture LoadCarTextureFromImage(CarImage carImage, Vector2 place, float scale)
{
	Texture2D body = LoadTextureFromImage(carImage.body);
	Texture2D leftWheel = LoadTextureFromImage(carImage.leftWheel);
	Texture2D rightWheel = LoadTextureFromImage(carImage.rightWheel);
	int frameWidth = carImage.body.width;
	int frameHeight = carImage.body.height;
	int wheelWidth = carImage.leftWheel.width;	 // 44
	int wheelHeight = carImage.leftWheel.height; // 110
	Vector2 wheelorigin = {.x = wheelWidth / 2, .y = wheelHeight / 2};
	Vector2 leftWheelOrigin = {((float)wheelWidth / 2.0f) * scale, ((float)wheelHeight / 2.0f) * scale};
	Vector2 rightWheelOrigin = {((float)wheelWidth / 2.0f) * scale, ((float)wheelHeight / 2.0f) * scale};
	float wheelY = 229.0f;
	float wheelX = 154.0f;
	Rectangle bodySourceRec = {0.0f, 0.0f, (float)frameWidth, (float)frameHeight};
	Rectangle bodyDestRec = {place.x, place.y, (float)frameWidth * scale, (float)frameHeight * scale};
	Rectangle leftWheelSourceRec = {0.0f, 0.0f, (float)wheelWidth, (float)wheelHeight};
	Rectangle leftWheelDestRec = {(-1 * (wheelX + leftWheelOrigin.x) * scale) + place.x, (-1 * (wheelY + leftWheelOrigin.y) * scale) + place.y, (float)wheelWidth * scale, (float)wheelHeight * scale};
	Rectangle rightWheelSourceRec = {0.0f, 0.0f, (float)wheelWidth, (float)wheelHeight};
	Rectangle rightWheelDestRec = {((wheelX + rightWheelOrigin.x) * scale) + place.x, (-1 * (wheelY + rightWheelOrigin.y) * scale) + place.y, (float)wheelWidth * scale, (float)wheelHeight * scale};
	Vector2 bodyOrigin = {((float)frameWidth / 2.0f) * scale, ((float)frameHeight / 2.0f) * scale};
	CarTexture carTexture = {
		.body = body,
		.bodySourceRec = bodySourceRec,
		.bodyDestRec = bodyDestRec,
		.bodyOrigin = bodyOrigin,
		.leftWheel = leftWheel,
		.leftWheelSourceRec = leftWheelSourceRec,
		.leftWheelDestRec = leftWheelDestRec,
		.leftWheelOrigin = leftWheelOrigin,
		.rightWheel = rightWheel,
		.rightWheelSourceRec = rightWheelSourceRec,
		.rightWheelDestRec = rightWheelDestRec,
		.rightWheelOrigin = rightWheelOrigin,
		.wheelRotation = carImage.wheelRotation,
		.carRotation = 0};

	return carTexture;
}
// function to load car with wheels
Image LoadCar(const char *carName, float scale)
{
	printf(carName);
	if (scale <= 0)
	{
		scale = 1.0f;
	}
	// Load a texture from the resources directory
	Image insignia = LoadImage(carName);

	Image wheel = LoadImage("wheel_black.png");
	Image car = LoadImage("insignia_empty.png");
	// ImageResize(&insignia, 100, 50);

	ImageScale(&wheel, scale, scale);
	ImageScale(&insignia, scale, scale);
	// ImageDraw(&insignia, wheel, (Rectangle){ 0, 0, (float)wheel.width, (float)wheel.height }, (Rectangle){ 20, 150, wheel.width, wheel.height }, WHITE);
	ImageDraw(&car, insignia, (Rectangle){0, 0, insignia.width, insignia.height}, (Rectangle){0, 0, (float)insignia.width, (float)insignia.height}, WHITE);
	int rear_wheel_height = 160 * scale;
	int right_wheel_width = 18 * scale;
	int left_wheel_width = 368 * scale;
	int front_wheel_height = 745 * scale;
	ImageDraw(&car, wheel, (Rectangle){0, 0, (float)wheel.width, (float)wheel.height}, (Rectangle){right_wheel_width, rear_wheel_height, wheel.width, wheel.height}, WHITE);
	ImageDraw(&car, wheel, (Rectangle){0, 0, (float)wheel.width, (float)wheel.height}, (Rectangle){left_wheel_width, rear_wheel_height, wheel.width, wheel.height}, WHITE);
	ImageDraw(&car, wheel, (Rectangle){0, 0, (float)wheel.width, (float)wheel.height}, (Rectangle){right_wheel_width, front_wheel_height, wheel.width, wheel.height}, WHITE);
	ImageDraw(&car, wheel, (Rectangle){0, 0, (float)wheel.width, (float)wheel.height}, (Rectangle){left_wheel_width, front_wheel_height, wheel.width, wheel.height}, WHITE);
	ImageDraw(&car, insignia, (Rectangle){0, 0, insignia.width, insignia.height}, (Rectangle){0, 0, (float)insignia.width, (float)insignia.height}, WHITE);
	UnloadImage(insignia);
	UnloadImage(wheel);
	ImageRotate(&car, 180);
	return car;
}

void RotateCar(CarTexture *carTexture, float carRotation, float carMovement, float scale)
{

	float xRotConstant = 320.0f * scale;
	float yRotConstant = 132.0f * scale;
	float xOriginConstant = 154.0f * scale;
	float yOriginConstant = 229.0f * scale;
	float xMovement = (carMovement * sinf(carRotation * DEG2RAD));
	float yMovement = (carMovement * cosf(carRotation * DEG2RAD));
	carTexture->bodyDestRec.x -= xMovement;
	carTexture->bodyDestRec.y += yMovement;
	carTexture->rightWheelDestRec.x = carTexture->bodyDestRec.x + cosf(carRotation * DEG2RAD) * (xOriginConstant + carTexture->leftWheelOrigin.x) + sinf(carRotation * DEG2RAD) * (xRotConstant - carTexture->leftWheelOrigin.x);
	carTexture->rightWheelDestRec.y = carTexture->bodyDestRec.y - cosf(carRotation * DEG2RAD) * (yOriginConstant + carTexture->leftWheelOrigin.y) + sinf(carRotation * DEG2RAD) * (yRotConstant + carTexture->leftWheelOrigin.y);

	carTexture->leftWheelDestRec.x = carTexture->bodyDestRec.x - cosf(carRotation * DEG2RAD) * (xOriginConstant + carTexture->leftWheelOrigin.x) + sinf(carRotation * DEG2RAD) * (xRotConstant - carTexture->leftWheelOrigin.x);
	carTexture->leftWheelDestRec.y = carTexture->bodyDestRec.y - cosf(carRotation * DEG2RAD) * (yOriginConstant + carTexture->leftWheelOrigin.y) - sinf(carRotation * DEG2RAD) * (yRotConstant + carTexture->leftWheelOrigin.y);
}

float MeasureParkDistance(CarTexture carTexture, float carAngle, TargetLocation target){
	
	Vector2 points1[5] = {0};
	GenerateCarEdges(carTexture.bodyDestRec, &carAngle, points1);
	DrawLineStrip(points1, 5, BLUE);
	// DrawRectangleRec(carTexture.bodyDestRec, BLUE);	
	Vector2 targetPoints[5] = {0};
	Rectangle targetRectangle = {.x = target.x, .y = target.y, .width = carTexture.bodyDestRec.width, .height = carTexture.bodyDestRec.height};
	GenerateCarEdges(targetRectangle, &target.rotation, targetPoints);
	DrawLineStrip(targetPoints, 5, BLUE);
	float totalDistance = 0;
	for(int i = 0; i < 4; i++){
		totalDistance += Vector2Distance(points1[i], targetPoints[i]);
	}
	return totalDistance;
}
CarTexture DrawCar(CarTexture *carTexture, float carRotation, float carMovement, float wheelRotation, float scale)
{
	RotateCar(carTexture, carRotation, carMovement, scale);
	DrawTexturePro(carTexture->rightWheel, carTexture->rightWheelSourceRec, carTexture->rightWheelDestRec, carTexture->rightWheelOrigin, (float)wheelRotation + carRotation, WHITE);
	DrawTexturePro(carTexture->leftWheel, carTexture->leftWheelSourceRec, carTexture->leftWheelDestRec, carTexture->leftWheelOrigin, (float)wheelRotation + carRotation, WHITE);
	DrawTexturePro(carTexture->body, carTexture->bodySourceRec, carTexture->bodyDestRec, carTexture->bodyOrigin, carRotation, WHITE);
}
// function to scale an image
void ImageScale(Image *image, float WidthScale, float HeightScale)
{
	if (HeightScale < 0)
	{
		HeightScale = WidthScale;
	}

	int newWidth = (float)image->width * WidthScale;
	int newHeight = (float)image->height * HeightScale;
	ImageResize(image, newWidth, newHeight);
}

bool CheckCollisionCars(CarTexture car1, float angle1, CarTexture car2, float angle2)
{

	Rectangle rotatedRec = car1.bodyDestRec;
	rotatedRec.x = rotatedRec.x - rotatedRec.width / 2;
	rotatedRec.y = rotatedRec.y - rotatedRec.height / 2;
	Rectangle rotatedRec2 = car2.bodyDestRec;
	rotatedRec2.x = rotatedRec2.x - rotatedRec2.width / 2;
	rotatedRec2.y = rotatedRec2.y - rotatedRec2.height / 2;
	Color color = RED;
	Vector2 points1[5] = {0};
	GenerateCarEdges(car1.bodyDestRec, &angle1, points1);
	Vector2 points1_[9] = {0};
	UltrasonicSensor sensors[8] = {0};
	GenerateCarEdges_(points1, points1_);
	GenerateSensorLocations(points1, sensors);
	// DrawLineStrip(points1, 5, color);
	DrawLineStrip(points1_, 9, color);
	Vector2 points2[5] = {0};
	Vector2 points2_[9] = {0};
	GenerateCarEdges(car2.bodyDestRec, &angle2, points2);
	GenerateCarEdges_(points2, points2_);
	Vector2 collisionPoint = {0};
	bool collided = false;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (CheckCollisionLines(points1_[i], points1_[i + 1], points2_[j], points2_[j + 1], &collisionPoint))
			{
				collided = true;
			}
		}
	}
	// DrawLineStrip(points2, 5, color);
	DrawLineStrip(points2_, 9, color);
	return collided;
}

SensorStatus CheckSensors(CarTexture car1, float angle1, CarTexture car2, float angle2, bool debug)
{

	Rectangle rotatedRec = car1.bodyDestRec;
	rotatedRec.x = rotatedRec.x - rotatedRec.width / 2;
	rotatedRec.y = rotatedRec.y - rotatedRec.height / 2;
	Rectangle rotatedRec2 = car2.bodyDestRec;
	rotatedRec2.x = rotatedRec2.x - rotatedRec2.width / 2;
	rotatedRec2.y = rotatedRec2.y - rotatedRec2.height / 2;
	Color color = RED;
	Vector2 points1[5] = {0};
	GenerateCarEdges(car1.bodyDestRec, &angle1, points1);
	Vector2 points1_[9] = {0};
	UltrasonicSensor sensors[8] = {0};
	GenerateCarEdges_(points1, points1_);
	GenerateSensorLocations(points1, sensors);
	// DrawLineStrip(points1, 5, color);
	if (debug)
		DrawLineStrip(points1_, 9, color);
	Vector2 points2[5] = {0};
	Vector2 points2_[9] = {0};
	GenerateCarEdges(car2.bodyDestRec, &angle2, points2);
	GenerateCarEdges_(points2, points2_);
	if(debug)
		DrawLineStrip(points2_, 9, color);
	Vector2 collisionPoint = {0};
	Vector2 sensorCollisionPoint = {0};
	bool collided = false;
	SensorStatus status = { .hit = false, .sensorHit = {-1}};
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (CheckCollisionLines(points1_[i], points1_[i + 1], points2_[j], points2_[j + 1], &collisionPoint))
			{
				collided = true;
			}
			if (CheckCollisionLines(sensors[i].location, sensors[i].middleRay, points2_[j], points2_[j + 1], &sensorCollisionPoint))
			{
				status.sensorHit[i] = Vector2Distance(sensors[i].location, sensorCollisionPoint) / Vector2Distance(sensors[i].location, sensors[i].middleRay);
				DrawLineV(sensors[i].location, sensorCollisionPoint, RED);
			}
		}
	}
	status.hit = collided;
	
	// DrawLineStrip(points2, 5, color);
	// DrawLineStrip(points2_, 9, color);
	return status;
}
void GenerateCarEdges_(Vector2 *oldPoints, Vector2 *points)
{
	float front_1 = 0.32f;
	float front_2 = 0.12f;
	float back_1 = 0.18f;
	float back_2 = 0.2f;

	points[0] = GetSplinePointLinear(oldPoints[0], oldPoints[1], front_1);	
	points[1] = GetSplinePointLinear(oldPoints[0], oldPoints[1], 1.0f - front_1);
	points[2] = GetSplinePointLinear(oldPoints[1], oldPoints[2], front_2);
	
	points[3] = GetSplinePointLinear(oldPoints[1], oldPoints[2], 1.0f - back_1);
	points[4] = GetSplinePointLinear(oldPoints[2], oldPoints[3], back_2);
	points[5] = GetSplinePointLinear(oldPoints[2], oldPoints[3], 1.0f - back_2);
	points[6] = GetSplinePointLinear(oldPoints[3], oldPoints[4], back_1);
	
	points[7] = GetSplinePointLinear(oldPoints[3], oldPoints[4], 1.0f - front_2);
	points[8] = points[0];
}


void GenerateCarEdges(Rectangle carRectangle, float *angle, Vector2 *points)
{
	Rectangle rotatedRec = carRectangle;
	rotatedRec.x = rotatedRec.x - rotatedRec.width / 2;
	rotatedRec.y = rotatedRec.y - rotatedRec.height / 2;

	float angleRad = *angle * DEG2RAD;
	points[0] = (Vector2){rotatedRec.x - cosf(angleRad) * rotatedRec.width / 2 + rotatedRec.width / 2 + sinf(angleRad) * rotatedRec.height / 2,
						  rotatedRec.y - cosf(angleRad) * rotatedRec.height / 2 + rotatedRec.height / 2 - sinf(angleRad) * rotatedRec.width / 2};
	points[1] = (Vector2){rotatedRec.x + cosf(angleRad) * rotatedRec.width / 2 + rotatedRec.width / 2 + sinf(angleRad) * rotatedRec.height / 2,
						  rotatedRec.y - cosf(angleRad) * rotatedRec.height / 2 + rotatedRec.height / 2 + sinf(angleRad) * rotatedRec.width / 2};
	points[2] = (Vector2){rotatedRec.x + cosf(angleRad) * rotatedRec.width / 2 + rotatedRec.width / 2 - sinf(angleRad) * rotatedRec.height / 2,
						  rotatedRec.y + rotatedRec.height / 2 + cosf(angleRad) * rotatedRec.height / 2 + sinf(angleRad) * rotatedRec.width / 2};
	points[3] = (Vector2){rotatedRec.x - cosf(angleRad) * rotatedRec.width / 2 + rotatedRec.width / 2 - sinf(angleRad) * rotatedRec.height / 2,
						  rotatedRec.y + rotatedRec.height / 2 + cosf(angleRad) * rotatedRec.height / 2 - sinf(angleRad) * rotatedRec.width / 2};
	points[4] = points[0];
}

// function to return a fixed timestep when debugging
float GetDeltaTime()
{
#ifdef _DEBUG
	return 1 / 60.0f;
#else
	return GetFrameTime();
#endif
}
