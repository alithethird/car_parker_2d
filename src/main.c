#include "raylib.h"
#include "stdlib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "helpers.h"
#include <stdio.h>
// #include "car_map.h"
#include "drive.h"
// #include <tensorflow/c/c_api.h>

int screenWidth = 800;
int screenHeight = 450;
const char *carList[] = {
	"insignia_blue.png",
	"insignia_green.png",
	"insignia_lightblue.png",
	"insignia_orange.png",
	"insignia_pink.png",
	"insignia_purple.png",
	"insignia_red.png",
	"insignia_yellow.png"};

void handleScreen()
{

	if (IsWindowResized() && !IsWindowFullscreen())
	{
		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();
	}

	// check for alt + enter
	if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
	{
		// see what display we are on right now
		int display = GetCurrentMonitor();

		if (IsWindowFullscreen())
		{
			// if we are full screen, then go back to the windowed size
			SetWindowSize(screenWidth, screenHeight);
		}
		else
		{
			// if we are not full screen, set the window size to match the monitor we are on
			SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
		}

		// toggle the state
		ToggleFullscreen();
	}
}
int main()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);
	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Left wheel is Warning");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	CarMap map = parseMap("map1.json");
	CarImage *carNewImageList = (CarImage *)malloc(sizeof(CarImage) * map.count);
	Vector2 *placeList = (Vector2 *)malloc(sizeof(Vector2) * map.count); //{(float)screenWidth / 2, (float)screenHeight / 2}; //{0,0};//
	CarTexture *carTextureList = (CarTexture *)malloc(sizeof(CarTexture) * map.count);
	for (int i = 0; i < map.count; i++)
	{
		carNewImageList[i] = LoadNewCar(map.carList[i].carFile); // carList[i]);
		placeList[i] = (Vector2){map.carList[i].x, map.carList[i].y};
		carTextureList[i] = LoadCarTextureFromImage(carNewImageList[i], placeList[i], map.carList[i].scale);
	}
	Drive carDrive = {0};

	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		{
			handleScreen();
			float speed = GetDeltaTime() * 300;
			float carRotationSpeed = GetDeltaTime() * 3;
			float wheelRotationSpeed = 0.5;

			carDrive.carMovement = 0;
			carDrive.rotationMovement = 0;
			if (IsKeyDown(KEY_RIGHT))
				rotateWheel(&carDrive, wheelRotationSpeed);
			if (IsKeyDown(KEY_LEFT))
				rotateWheel(&carDrive, -wheelRotationSpeed);
			if (IsKeyDown(KEY_UP))
				moveCar(&carDrive, -speed, -carRotationSpeed);
			if (IsKeyDown(KEY_DOWN))
				moveCar(&carDrive, speed, carRotationSpeed);
			if (IsKeyDown(KEY_R))
				resetWheel(&carDrive);

			// drawing
			BeginDrawing();
		}
		ClearBackground(LIGHTGRAY);

		carDrive.carRotation += (float)carDrive.wheelRotation * carDrive.rotationMovement * -1;
		DrawCar(&carTextureList[0], carDrive.carRotation, carDrive.carMovement, carDrive.wheelRotation, map.carList[0].scale);
		bool col = false;
		float targetDistance = 0;
		/*
		nn

		inputs:
			- sensorStatus:
				* bool hit
				* float sensorHit
			- float targetDistance
			- Drive:
				* float carMovement
				* float rotationMovement
				* float wheelRotation
				* float carRotation
		
		outputs:
			- float wheelRotationSpeed
			- float carRotationSpeed
			- float speed
		*/

		for (int i = 1; i < map.count; i++)
		{
			DrawCar(&carTextureList[i], 0, 0, 0, map.carList[i].scale);
			col = (CheckSensors(carTextureList[0], carDrive.carRotation, carTextureList[i], 0, false).hit) ? true : col;
		}
			targetDistance = MeasureParkDistance(carTextureList[0], carDrive.carRotation, map.target);
		char* distance[20];
		sprintf(distance, "%.4f", targetDistance);
			DrawText(distance, 200, 600, 200, RED);
		if (col)
			DrawText("COLLISION", 200, 200, 200, RED);
		EndDrawing();
	}
	// cleanup
	// unload our texture so it can be cleaned up

	for (int i = 0; i < map.count; i++)
	{
		UnloadTexture(carTextureList[i].leftWheel);
		UnloadTexture(carTextureList[i].rightWheel);
		UnloadTexture(carTextureList[i].body);
	}
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
