/*******************************************************************************************
*
*   raylib [models] example - Waving cubes
*
*   This example has been created using raylib 2.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Example contributed by Codecat (@codecat) and reviewed by Ramon Santamaria (@raysan5)
*
*   Copyright (c) 2019 Codecat (@codecat) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "mouse.h"

#include <stdlib.h>
#include <math.h>

#define PI 3.141592
#define PLAYER_MOVEMENT_SENSITIVITY 20

int mapheights[1024] = { 0 };
Vector3 playerPosition = (Vector3){0};

float calculateLightLevel(Vector3 cubePos, Vector3 lightSource, int strength) {
	float x = cubePos.x - lightSource.x;
	float y = cubePos.y - lightSource.y;
	float z = cubePos.z - lightSource.z;

	if(x < 0) {
		x = -x;
	}

	if(y < 0) {
		y = -y;
	}

	if(z < 0) {
		z = -z;
	}

	float total = x + y + z;

	if(total < strength / 1.95) return 1;
	if(total < strength / 1.9) return 0.95;
	if(total < strength / 1.85) return 0.9;
	if(total < strength / 1.8) return 0.85;
	if(total < strength / 1.75) return 0.8;
	if(total < strength / 1.7) return 0.75;
	if(total < strength / 1.65) return 0.7;
	if(total < strength / 1.6) return 0.65;
	if(total < strength / 1.55) return 0.6;
	if(total < strength / 1.5) return 0.5;
	if(total < strength / 1.45) return 0.45;
	if(total < strength / 1.4) return 0.4;
	if(total < strength / 1.35) return 0.35;
	if(total < strength / 1.3) return 0.3;
	if(total < strength / 1.25) return 0.25;
	if(total < strength / 1.2) return 0.2;
	if(total < strength / 1.15) return 0.15;
	if(total < strength / 1.1) return 0.1;
	if(total < strength / 1.05) return 0.05;
	if(total < strength) return 0.035;
	

	return 0.035;
}

Vector3 playerRotation = (Vector3){0, -20, 0};

void UpdateGameCamera(Camera *camera, Vector3 *playerPos)
{
	bool direction[6] = {IsKeyDown(KEY_W),
	             IsKeyDown(KEY_S),
	             IsKeyDown(KEY_D),
	             IsKeyDown(KEY_A),
	             IsKeyDown(KEY_E),
	             IsKeyDown(KEY_Q)};

	playerPos->x += (sinf(playerRotation.x)*direction[1] -
	                 sinf(playerRotation.x)*direction[0] -
	                 cosf(playerRotation.x)*direction[3] +
	                 cosf(playerRotation.x)*direction[2])/PLAYER_MOVEMENT_SENSITIVITY;
						
	playerPos->y += (1.0f*direction[4] - 1.0f*direction[5])/PLAYER_MOVEMENT_SENSITIVITY;
	
	playerPos->z += (cosf(playerRotation.x)*direction[1] -
	                 cosf(playerRotation.x)*direction[0] +
	                 sinf(playerRotation.x)*direction[3] -
	                 sinf(playerRotation.x)*direction[2])/PLAYER_MOVEMENT_SENSITIVITY;

    camera->position.x = playerPos->x;
    
    // Note: place camera on top of player's head.
    camera->position.y = playerPos->y;
    camera->position.z = playerPos->z;
    
    playerRotation.y -= GetMouseMovement().y;
    playerRotation.x -= GetMouseMovement().x;

	if(playerRotation.y < -20.35) {
		playerRotation.y = -20.35;
	}

	if(playerRotation.y > -17.30) {
		playerRotation.y = -17.30;
	}

    Matrix translation = MatrixTranslate(0, 0, 1);
    Matrix rotation = MatrixRotateXYZ((Vector3){PI * 2 - playerRotation.y, PI * 2 - playerRotation.x, 0});
    Matrix transform = MatrixMultiply(translation, rotation);

    camera->target.x = camera->position.x - transform.m12;
    camera->target.y = camera->position.y - transform.m13;
    camera->target.z = camera->position.z - transform.m14;
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "minecraft clone");

    // Initialize the camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 1.0f, 0.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 80.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type

    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    // Specify the amount of blocks in each direction
    const int chunkSize = 16;
    const int maxHeight = 10;
    int renderDistance = 2;

    for(int i = 0; i < 1024; i++) {
    	if(i == 0) {
    		mapheights[i] = rand() % maxHeight;
    	} else {
    		float num = rand() % 100;
    		
    		if(num < 50 && num > 40) {
    			mapheights[i] = mapheights[i - 1] - (rand() % rand() % 2);
    		} else if(num > 50 && num < 60) {
    			mapheights[i] = mapheights[i - 1] + (rand() % rand() % 2);
    		} else {
    			mapheights[i] = mapheights[i - 1];
    		}
    	}

    	if(mapheights[i] < 1) {
    		mapheights[i] = 1;
    	}
    }

    SetTargetFPS(100);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
		UpdateMouseMovement();
    
        // Update
        //----------------------------------------------------------------------------------
        UpdateGameCamera(&camera, &playerPosition);          // Update camera

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                for (int x = 0; x < chunkSize * renderDistance; x++)
                {
                    for (int z = 0; z < chunkSize * renderDistance; z++)
                    {
                        for (int y = 0; y < mapheights[x * z]; y++)
                        {
							// Calculate cube size
							float cubeSize = 0.45;
                        
							// Calculate the cube position
                            Vector3 cubePos = {
                                (float)(x - chunkSize/2) * cubeSize,
                                (float)(y - chunkSize/2) * cubeSize,
                                (float)(z - chunkSize/2) * cubeSize,
                            };
	
                            Color color = GREEN;

                            Vector3 colorVector = ColorToHSV(color);

                            colorVector.z = calculateLightLevel(cubePos, camera.position, 20);

                            Color newColor = ColorFromHSV(colorVector.x, colorVector.y, colorVector.z); 

                            Color color2 = BROWN;
                            
                            Vector3 colorVector2 = ColorToHSV(color2);
                            
                            colorVector2.z = calculateLightLevel(cubePos, camera.position, 20);
                            
                            Color newColor2 = ColorFromHSV(colorVector2.x, colorVector2.y, colorVector2.z); 

                            // And finally, draw the cube!
                            if(y == (mapheights[x * z]) - 1) {
                            	DrawCube(cubePos, cubeSize, cubeSize, cubeSize, newColor);
                            } else {
                            	DrawCube(cubePos, cubeSize, cubeSize, cubeSize, newColor2);
                            }
                        }
                    }
                }

            EndMode3D();

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
