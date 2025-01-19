#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include <stdio.h>
#define LIMIT_SHORT 0.1f
#define LIMIT_MIDDLE 0.2f
#define LIMIT_FAR 0.3f

#define FRONT_INNER_ANGLE 0.0f
#define FRONT_OUTER_ANGLE 30.0f
#define REAR_INNER_ANGLE 0.0f
#define REAR_OUTER_ANGLE 30.0f

typedef struct UltrasonicSensor
{
    Vector2 location;
    float angle;
    Vector2 leftRay;
    Vector2 middleRay;
    Vector2 rightRay;

} UltrasonicSensor;

Vector2 RotatePoint(Vector2 o, float angleInRads, Vector2 p)
{
    float s = sinf(angleInRads);
    float c = cosf(angleInRads);

    // translate point back to origin:
    p.x -= o.x;
    p.y -= o.y;

    // rotate point
    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;

    // translate point back:
    p.x = xnew + o.x;
    p.y = ynew + o.y;

    return p;
}

UltrasonicSensor RotateSensor(UltrasonicSensor sensor)
{
    float radAngle = sensor.angle * DEG2RAD;
    sensor.middleRay = RotatePoint(sensor.location, radAngle, sensor.middleRay);
    return sensor;
}

void DrawUltrasonicSensor(UltrasonicSensor sensor)
{
    float rad = 5.0f;
    DrawCircle(sensor.location.x, sensor.location.y, rad, BLACK);
    DrawLineEx(sensor.location, sensor.middleRay, 2.0f, ORANGE);
}

void GenerateSensorLocations(Vector2 *points, UltrasonicSensor *sensors)
{
    float frontOuterX = 0.16f;
    float frontInnerX = 0.36f;
    float rearOuterX = 0.2f;
    float rearInnerX = 0.34f;
    float frontOuterY = 0.06f;
    float frontInnerY = 0.01f;
    float rearOuterY = 0.03f;
    float rearInnerY = 0.01f;

    // Front sensors X axis calculation
    sensors[0].location = GetSplinePointLinear(points[0], points[1], frontOuterX);
    sensors[1].location = GetSplinePointLinear(points[0], points[1], frontInnerX);
    sensors[2].location = GetSplinePointLinear(points[0], points[1], 1.0f - frontInnerX);
    sensors[3].location = GetSplinePointLinear(points[0], points[1], 1.0f - frontOuterX);

    // Rear sensors X axis calculation
    sensors[4].location = GetSplinePointLinear(points[2], points[3], rearOuterX);
    sensors[5].location = GetSplinePointLinear(points[2], points[3], rearInnerX);
    sensors[6].location = GetSplinePointLinear(points[2], points[3], 1.0f - rearInnerX);
    sensors[7].location = GetSplinePointLinear(points[2], points[3], 1.0f - rearOuterX);

    // Front sensors Y axis calculation
    Vector2 tempVector = GetSplinePointLinear(points[3], points[2], frontOuterX);
    sensors[0].location = GetSplinePointLinear(sensors[0].location, tempVector, frontOuterY);
    sensors[0].middleRay = GetSplinePointLinear(sensors[0].location, tempVector, -LIMIT_FAR);
    tempVector = GetSplinePointLinear(points[3], points[2], frontInnerX);
    sensors[1].location = GetSplinePointLinear(sensors[1].location, tempVector, frontInnerY);
    sensors[1].middleRay = GetSplinePointLinear(sensors[1].location, tempVector, -LIMIT_FAR);
    tempVector = GetSplinePointLinear(points[2], points[3], frontInnerX);
    sensors[2].location = GetSplinePointLinear(sensors[2].location, tempVector, frontInnerY);
    sensors[2].middleRay = GetSplinePointLinear(sensors[2].location, tempVector, -LIMIT_FAR);
    tempVector = GetSplinePointLinear(points[2], points[3], frontOuterX);
    sensors[3].location = GetSplinePointLinear(sensors[3].location, tempVector, frontOuterY);
    sensors[3].middleRay = GetSplinePointLinear(sensors[3].location, tempVector, -LIMIT_FAR);

    // Rear sensors Y axis calculation
    tempVector = GetSplinePointLinear(points[1], points[0], rearOuterX);
    sensors[4].location = GetSplinePointLinear(sensors[4].location, tempVector, rearOuterY);
    sensors[4].middleRay = GetSplinePointLinear(sensors[4].location, tempVector, -LIMIT_FAR);
    tempVector = GetSplinePointLinear(points[1], points[0], rearInnerX);
    sensors[5].location = GetSplinePointLinear(sensors[5].location, tempVector, rearInnerY);
    sensors[5].middleRay = GetSplinePointLinear(sensors[5].location, tempVector, -LIMIT_FAR);
    tempVector = GetSplinePointLinear(points[0], points[1], rearInnerX);
    sensors[6].location = GetSplinePointLinear(sensors[6].location, tempVector, rearInnerY);
    sensors[6].middleRay = GetSplinePointLinear(sensors[6].location, tempVector, -LIMIT_FAR);
    tempVector = GetSplinePointLinear(points[0], points[1], rearOuterX);
    sensors[7].location = GetSplinePointLinear(sensors[7].location, tempVector, rearOuterY);
    sensors[7].middleRay = GetSplinePointLinear(sensors[7].location, tempVector, -LIMIT_FAR);

    // set angles

    // front
    // outer
    sensors[0].angle = -FRONT_OUTER_ANGLE;
    sensors[3].angle = FRONT_OUTER_ANGLE;
    // inner
    sensors[1].angle = -FRONT_INNER_ANGLE;
    sensors[2].angle = FRONT_INNER_ANGLE;

    // rear
    // outer
    sensors[4].angle = -REAR_OUTER_ANGLE;
    sensors[7].angle = REAR_OUTER_ANGLE;
    // inner
    sensors[5].angle = -REAR_INNER_ANGLE;
    sensors[6].angle = REAR_INNER_ANGLE;
    // Calculate middle Rays
    

    for (int i = 0; i < 8; i++)
    {
        sensors[i] = RotateSensor(sensors[i]);
        DrawUltrasonicSensor(sensors[i]);
    }
}
