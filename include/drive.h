
#include "raylib.h"
#include "stdlib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include <stdio.h>

#define MAX_WHEEL_ANGLE 25

typedef struct Drive
{
    float carMovement;
    float rotationMovement;
    float wheelRotation;
    float carRotation;
} Drive;

void rotateWheel(Drive *carDrive, float rotationSpeed)
{
    carDrive->wheelRotation += rotationSpeed;
    if (carDrive->wheelRotation >= MAX_WHEEL_ANGLE)
        carDrive->wheelRotation = MAX_WHEEL_ANGLE;
    if (carDrive->wheelRotation <= -MAX_WHEEL_ANGLE)
        carDrive->wheelRotation = -MAX_WHEEL_ANGLE;
}

void resetWheel(Drive *carDrive)
{
    carDrive->wheelRotation = 0;
}

void moveCar(Drive *carDrive, float speed, float rotationSpeed)
{
    carDrive->carMovement += speed;
    carDrive->rotationMovement += rotationSpeed;
}