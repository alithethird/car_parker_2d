#include "raylib.h"
#include "stdlib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "helpers.h"
#include <stdio.h>

// #include <tensorflow/c/c_api.h>

#define CAR_ARRAY_SIZE 2
#define MAX_WHEEL_ANGLE 25
#define SCALE 0.4f

const char *carList[] = {
	"insignia_blue.png",
	"insignia_green.png",
	"insignia_lightblue.png",
	"insignia_orange.png",
	"insignia_pink.png",
	"insignia_purple.png",
	"insignia_red.png",
	"insignia_yellow.png"};

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
	int placeX = 1000;
	int placeY = 1300;
	int placeXIncrement = 2500;
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

	// printf("Hello from TensorFlow C library version %s\n", TF_Version());
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
		if (CheckCollisionCars(carTextureList[0], carRotation, carTextureList[1], carRotation))
		{
			DrawText("COLLISION", 200, 200, 200, RED);
		}
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
