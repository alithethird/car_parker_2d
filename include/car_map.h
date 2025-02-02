
#include "raylib.h"
#include "stdlib.h"
#include <stdio.h>
#include "jsmn.h"
#include <string.h>
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

typedef struct CarLocation
{
    int x;
    int y;
    float scale;
    char carName[20];
    char carFile[20];
} CarLocation;

typedef struct TargetLocation
{
    int x;
    int y;
    float scale;
    float rotation;
} TargetLocation;

typedef struct CarMap
{
    CarLocation *carList;
    TargetLocation target;
    int count;
    char mapLocation[20];
} CarMap;

void PrintCarLocation(CarLocation carLocation)
{
    printf("CAR name:%s, x: %d, y: %d, scale: %f, file: %s\n", carLocation.carName, carLocation.x, carLocation.y, carLocation.scale, carLocation.carFile);
}

void PrintCarMap(CarMap carMap)
{
    printf("CARMAP mapLocation: %s   count: %d\n", carMap.mapLocation, carMap.count);
    for (int i = 0; i < carMap.count; i++)
    {
        printf("  - ");
        PrintCarLocation(carMap.carList[i]);
    }
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}

static int jsonStartWith(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int)strlen(s) <= tok->end - tok->start &&
        strncmp(json + tok->start, s, (int)strlen(s)) == 0)
    {
        return 0;
    }
    return -1;
}


// static const char *JSON_STRING =
//     "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
//     "\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";
// static const char *JSON_STRING = "{\"car1\": {\"x\": 100, \"y\": 200, \"scale\": 0.4},\"car2\": {\"x\": 300, \"y\": 200, \"scale\": 0.4},\"car3\": {\"x\": 500, \"y\": 200, \"scale\": 0.4},\"car4\": {\"x\": 700, \"y\": 200, \"scale\": 0.4}}";

CarMap parseMap(const char *mapLocation)
{

    char fullLocation[80];
    strcpy(fullLocation, "maps/");
    strcat(fullLocation, mapLocation);
    int dataSize = 0;
    FILE *fptr;
    char *JSON_STRING = (char *)LoadFileData(fullLocation, &dataSize);

    JSON_STRING[dataSize] = '\0';
    printf("%s\n", JSON_STRING);
    printf("%d\n", dataSize);
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[128]; /* We expect no more than 128 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                   sizeof(t) / sizeof(t[0]));
    if (r < 0)
    {
        printf("Failed to parse JSON: %d\n", r);
        exit(1);
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        printf("Object expected\n");
        exit(1);
    }
    CarLocation *carLocations;
    TargetLocation target;
    carLocations = malloc(4 * sizeof(CarLocation));
    CarMap carMap = {0, .mapLocation = mapLocation, .carList = carLocations, .count = 4, .target=target};
    strcpy(carMap.mapLocation, mapLocation);
    int carIndex = 0;
    char carName[20] = {'\0'};
    int tempX = 0;
    int tempY = 0;
    float tempScale = 0;
    char tempName[20];
    char tempFile[20];
    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++)
    {
        if (jsonStartWith(JSON_STRING, &t[i], "car") == 0)
        {
            int j;
            jsmntok_t *g = &t[i];
            sprintf(carName, "%.*s", g->end - g->start, JSON_STRING + g->start);
            printf("- %s\n", carName);
            sprintf(tempName, carName);
            if (t[i + 1].type != JSMN_OBJECT)
            {
                continue; /* We expect car to be an object */
            }
            char tokenName[10] = {'\0'};
            char tokenValue[20] = {'\0'};
            for (j = 0; j < t[i + 1].size * 2; j++)
            {
                jsmntok_t *g = &t[i + j + 2];
                if (j % 2 == 0)
                {
                    sprintf(tokenName, "%.*s\0", g->end - g->start, JSON_STRING + g->start);
                    printf("  * %s:", tokenName);
                }
                else
                {
                    sprintf(tokenValue, "%.*s\0", g->end - g->start, JSON_STRING + g->start);
                    printf(" %s\n", tokenValue);
                    if (strncmp(tokenName, "x", 2) == 0)
                    {
                        tempX = atoi(tokenValue);
                    }
                    else if (strncmp(tokenName, "y", 1) == 0)
                    {
                        tempY = atoi(tokenValue);
                    }
                    else if (strncmp(tokenName, "scale", 5) == 0)
                    {
                        tempScale = atof(tokenValue);
                    }
                    else if (strncmp(tokenName, "name", 4) == 0)
                    {
                        strcpy(tempFile, tokenValue);
                    }
                }
            }
            i += t[i + 1].size + 1;
            CarLocation tempCar = {.x = tempX, .y = tempY, .scale = tempScale, .carFile = tempFile, .carName = tempName};
            strcpy(tempCar.carFile, tempFile);
            strcpy(tempCar.carName, tempName);
            carLocations[carIndex] = tempCar;
            carIndex++;
        }
        else if (jsonStartWith(JSON_STRING, &t[i], "target") == 0)
        {
            int j;
            jsmntok_t *g = &t[i];
            if (t[i + 1].type != JSMN_OBJECT)
            {
                continue; /* We expect car to be an object */
            }
            char tokenName[10] = {'\0'};
            char tokenValue[20] = {'\0'};
            float tempRotation = 0;
            for (j = 0; j < t[i + 1].size * 2; j++)
            {
                jsmntok_t *g = &t[i + j + 2];
                if (j % 2 == 0)
                {
                    sprintf(tokenName, "%.*s\0", g->end - g->start, JSON_STRING + g->start);
                    printf("  * %s:", tokenName);
                }
                else
                {
                    sprintf(tokenValue, "%.*s\0", g->end - g->start, JSON_STRING + g->start);
                    printf(" %s\n", tokenValue);
                    if (strncmp(tokenName, "x", 2) == 0)
                    {
                        tempX = atoi(tokenValue);
                    }
                    else if (strncmp(tokenName, "y", 1) == 0)
                    {
                        tempY = atoi(tokenValue);
                    }
                    else if (strncmp(tokenName, "scale", 5) == 0)
                    {
                        tempScale = atof(tokenValue);
                    }
                    else if (strncmp(tokenName, "rotation", 8) == 0)
                    {
                        tempRotation = atof(tokenValue);
                    }
                }
            }
            i += t[i + 1].size + 1;
            TargetLocation targ = {.x=tempX, .y=tempY, .scale=tempScale, .rotation=tempRotation};
            carMap.target = targ;
        }
        else
        {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                   JSON_STRING + t[i].start);
        }
    }
    PrintCarMap(carMap);
    return carMap;
}