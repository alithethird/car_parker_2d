#include "raylib.h"
#include "stdlib.h"
#include <stdio.h>
#include "jsmn.h"
#include <string.h>
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

typedef struct CarHistory
{
    int fps;
    float speed;
    float rotationSpeed;
    float wheelRotationSpeed;
    int *movement;
    int movementCount;
} CarHistory;

void PrintCarHistory(CarHistory carHistory)
{
    printf("HISTORY fps:%d, speed: %f, rotationSpeed: %f, wheelRotationSpeed: %f", carHistory.fps, carHistory.speed, carHistory.rotationSpeed, carHistory.wheelRotationSpeed);
    printf("\nMovement: ");
    for (int mi = 0; mi < carHistory.movementCount; mi++)
    {
        printf("%d, ", carHistory.movement[mi]);
    }
}

void parseHistory(const char *historyLocation, int *arr_size, CarHistory *carHistories)
{

    char fullLocation[80];
    strcpy(fullLocation, "histories/");
    strcat(fullLocation, historyLocation);
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
    carHistories = (CarHistory *)malloc(*arr_size * sizeof(CarHistory));
    // strcpy(carMap.historyLocation, mapLocation);
    int historyIndex = 0;
    int movementSixe = 8;
    /* Loop over all keys of the root object */
    char historyName[20];
    char tempName[20];
    for (i = 1; i < r; i++)
    {
        if (jsonStartWith(JSON_STRING, &t[i], "history") == 0)
        {
            if (historyIndex >= *arr_size)
            {
                *arr_size = *arr_size + 1;
                CarHistory *newCarHistories = (CarHistory *)malloc(*arr_size * sizeof(CarHistory));
                for (int k = 0; k < historyIndex; k++)
                {
                    newCarHistories[k] = carHistories[k];
                }
                free(carHistories);
                carHistories = newCarHistories;
            }
            int j;
            jsmntok_t *g = &t[i];
            sprintf(historyName, "%.*s", g->end - g->start, JSON_STRING + g->start);
            printf("- %s\n", historyName);
            sprintf(tempName, historyName);
            if (t[i + 1].type != JSMN_OBJECT)
            {
                continue; /* We expect car to be an object */
            }
            char tokenName[20] = {'\0'};
            char tokenValue[20] = {'\0'};
            float tempFps = 0;
            float tempSpeed = 0;
            float tempRotation = 0;
            float tempWheelRotation = 0;
            long movementSize = 100;
            int *movementList = (int *)malloc(movementSize * sizeof(int));
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
                    if (strncmp(tokenName, "fps", 3) == 0)
                    {
                        tempFps = atoi(tokenValue);
                    }
                    else if (strncmp(tokenName, "speed", 5) == 0)
                    {
                        tempSpeed = atof(tokenValue);
                    }
                    else if (strncmp(tokenName, "rotation_speed", 8) == 0)
                    {
                        tempRotation = atof(tokenValue);
                    }
                    else if (strncmp(tokenName, "wheel_rotation_speed", 14) == 0)
                    {
                        tempWheelRotation = atof(tokenValue);
                    }
                    else if (strncmp(tokenName, "movement", 8) == 0)
                    {
                        char tempMovement[3] = {'\0'};
                        long movementIndex = 0;
                        for (int kk = 0; kk < g->size; kk++)
                        {
                            jsmntok_t *arrTok = &t[i + j + kk + 3];
                            if (arrTok->type == JSMN_PRIMITIVE)
                            {
                                // printf("\n===%.*s===", arrTok->end - arrTok->start, JSON_STRING + arrTok->start);
                                sprintf(tempMovement, "%.*s", arrTok->end - arrTok->start, JSON_STRING + arrTok->start);
                                movementList[movementIndex] = atoi(tempMovement);
                                movementIndex++;
                        if (movementIndex > movementSize)
                        {
                            movementSize = movementIndex;
                            int *tempList = (int *)malloc(movementIndex * sizeof(int)*1.2);
                            for (int ii = 0; ii <= movementIndex; ii++)
                            {
                                tempList[ii] = movementList[ii];
                            }
                            free(movementList);
                            movementList = tempList;
                        }
                            }
                        }
                    }
                }
            }
            i += t[i + 1].size + 1;
            CarHistory tempHistory = {.fps = tempFps, .speed = tempSpeed, .rotationSpeed = tempRotation, .wheelRotationSpeed = tempWheelRotation, .movement = movementList, .movementCount = movementSize};
            carHistories[historyIndex] = tempHistory;
            historyIndex++;
        }
        else
        {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                   JSON_STRING + t[i].start);
        }
    }
    // printf("Car histories:");
    // for (int i = 0; i < historyIndex; i++)
    // {
    //     printf("\n\n  - ");
    //     PrintCarHistory(carHistories[i]);
    // }
}