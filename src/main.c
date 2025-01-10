/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you
--  wrote the original software. If you use this software in a product, an acknowledgment
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "raylib.h"
#include "stdlib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "helpers.h"

const char *carList[] = {
	"insignia_blue.png",
	"insignia_green.png",
	"insignia_lightblue.png",
	"insignia_orange.png",
	"insignia_pink.png",
	"insignia_purple.png",
	"insignia_red.png",
	"insignia_yellow.png"};
#define CAR_ARRAY_SIZE 6
#define MAX_WHEEL_ANGLE 25
#define SCALE 0.40f

int main()
{
	printf("HELOO\n");
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);
	int screenWidth = 800;
	int screenHeight = 450;
	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Left wheel is Warning");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	CarImage carNewImageList[CAR_ARRAY_SIZE] = {0};
	Vector2 placeList[CAR_ARRAY_SIZE] = {0}; //{(float)screenWidth / 2, (float)screenHeight / 2}; //{0,0};//
	CarTexture carTextureList[CAR_ARRAY_SIZE] = {0};
	int placeX = 500;
	int placeY = 300;
	int placeXIncrement = 300;
	int placeYIncrement = 20;
	for (int i = 0; i < CAR_ARRAY_SIZE; i++)
	{
		carNewImageList[i] = LoadNewCar(carList[i]);
		if (i == (CAR_ARRAY_SIZE / 2))
		{
			placeX += placeXIncrement;
			placeY = 300;
		}
		placeList[i] = (Vector2){placeX, placeY};
		carTextureList[i] = LoadCarTextureFromImage(carNewImageList[i], placeList[i], SCALE);
		placeY += (carNewImageList[i].body.height * SCALE + placeYIncrement);
		// UnloadImage(newCar);
	}
	CarImage car_2 = LoadNewCar("insignia_orange.png");
	float carMovement = 0;
	float rotationMovement = 0;
	Vector2 place_1 = {500.0f, 700.0f};	 //{(float)screenWidth / 2, (float)screenHeight / 2}; //{0,0};//
	Vector2 place_2 = {1500.0f, 700.0f}; //{(float)screenWidth / 2, (float)screenHeight / 2}; //{0,0};//
	float wheelRotation = 0;
	float carRotation = 0;

	int destX = 0;
	int destY = 0;

	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
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
			float speed = GetDeltaTime() * 300;
			float carRotationSpeed = GetDeltaTime() * 3;
			float rotationSpeed = 0.5;

			carMovement = 0;
			rotationMovement = 0;
			// wheelRotation = 0;
			if (IsKeyDown(KEY_RIGHT))
				wheelRotation += rotationSpeed;
			if (IsKeyDown(KEY_LEFT))
				wheelRotation -= rotationSpeed;
			if (IsKeyDown(KEY_UP))
			{
				carMovement -= speed;
				rotationMovement -= carRotationSpeed;
			}
			if (IsKeyDown(KEY_DOWN))
			{
				carMovement += speed;
				rotationMovement += carRotationSpeed;
			}
			if (IsKeyDown(KEY_R))
			{
				wheelRotation = 0;
			}

			if (wheelRotation >= MAX_WHEEL_ANGLE)
				wheelRotation = MAX_WHEEL_ANGLE;
			if (wheelRotation <= -MAX_WHEEL_ANGLE)
				wheelRotation = -MAX_WHEEL_ANGLE;

			// drawing
			BeginDrawing();
		}
		// Setup the back buffer for drawing (clear color and depth buffers)

		ClearBackground(LIGHTGRAY);

		// draw some text using the default font
		DrawText("Left wheel is Warning", 000, 200, 20, WHITE);

		carRotation += (float)wheelRotation * rotationMovement * -1;
		printf("carRotation:%f\n", carRotation);
		// DrawCar(&carTexture_1, carRotation, carMovement, wheelRotation, SCALE);

		for (int i = 0; i < CAR_ARRAY_SIZE; i++)
		{
			DrawCar(&carTextureList[i], carRotation, carMovement, wheelRotation, SCALE);
		}
		printf("carRotation2:%f\n", carRotation);
		// if (CheckCollisionCars(carTexture_1, carRotation, carTexture_2, 0))
		// {
		// 	DrawText("COLLISION", 200, 200, 200, RED);
		// }
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}
	// cleanup
	// unload our texture so it can be cleaned up

	for (int i = 0; i < CAR_ARRAY_SIZE; i++)
	{
		UnloadTexture(carTextureList[i].leftWheel);
		UnloadTexture(carTextureList[i].rightWheel);
		UnloadTexture(carTextureList[i].body);
	}
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
