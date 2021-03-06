#include "raylib.h"
#include "raymath.h"
#include "mouse.h"

#include <stdlib.h>
#include <math.h>

#define PLAYER_MOVEMENT_SENSITIVITY 20

// will be replaced with perlin noise eventually
int mapheights[1024] = {0};
const int chunkSize = 16;
int renderDistance = 2;

Vector3 playerPosition = (Vector3){ 0 };

// very dumb
float calculateLightLevel(Vector3 cubePos, Vector3 lightSource, int strength)
{
    float x = cubePos.x - lightSource.x;
    float y = cubePos.y - lightSource.y;
    float z = cubePos.z - lightSource.z;

    if (x < 0)
    {
        x = -x;
    }

    if (y < 0)
    {
        y = -y;
    }

    if (z < 0)
    {
        z = -z;
    }

    float total = x + y + z;

    // wow i sure hate the code below
    if (total < strength / 1.95)
        return 1;
    if (total < strength / 1.9)
        return 0.95;
    if (total < strength / 1.85)
        return 0.9;
    if (total < strength / 1.8)
        return 0.85;
    if (total < strength / 1.75)
        return 0.8;
    if (total < strength / 1.7)
        return 0.75;
    if (total < strength / 1.65)
        return 0.7;
    if (total < strength / 1.6)
        return 0.65;
    if (total < strength / 1.55)
        return 0.6;
    if (total < strength / 1.5)
        return 0.5;
    if (total < strength / 1.45)
        return 0.45;
    if (total < strength / 1.4)
        return 0.4;
    if (total < strength / 1.35)
        return 0.35;
    if (total < strength / 1.3)
        return 0.3;
    if (total < strength / 1.25)
        return 0.25;
    if (total < strength / 1.2)
        return 0.2;
    if (total < strength / 1.15)
        return 0.15;
    if (total < strength / 1.1)
        return 0.1;
    if (total < strength / 1.05)
        return 0.05;
    if (total < strength)
        return 0.04;

    return 0.04;
}

Vector3 playerRotation = (Vector3){0, -20, 0};

int selectedPosition = 0;

void UpdateGameCamera(Camera *camera, Vector3 *playerPos)
{
    bool direction[6] = {IsKeyDown(KEY_W),
                         IsKeyDown(KEY_S),
                         IsKeyDown(KEY_D),
                         IsKeyDown(KEY_A),
                         IsKeyDown(KEY_E),
                         IsKeyDown(KEY_Q)};

    playerPos->x += (sinf(playerRotation.x) * direction[1] -
                     sinf(playerRotation.x) * direction[0] -
                     cosf(playerRotation.x) * direction[3] +
                     cosf(playerRotation.x) * direction[2]) /
                    PLAYER_MOVEMENT_SENSITIVITY;

    playerPos->y += (1.0f * direction[4] - 1.0f * direction[5]) / PLAYER_MOVEMENT_SENSITIVITY;

    playerPos->z += (cosf(playerRotation.x) * direction[1] -
                     cosf(playerRotation.x) * direction[0] +
                     sinf(playerRotation.x) * direction[3] -
                     sinf(playerRotation.x) * direction[2]) /
                    PLAYER_MOVEMENT_SENSITIVITY;

    camera->position.x = playerPos->x;

    // Note: place camera on top of player's head.
    camera->position.y = playerPos->y;
    camera->position.z = playerPos->z;

    playerRotation.y -= GetMouseMovement().y;
    playerRotation.x -= GetMouseMovement().x;

    if (playerRotation.y < -20.35)
    {
        playerRotation.y = -20.35;
    }

    if (playerRotation.y > -17.30)
    {
        playerRotation.y = -17.30;
    }

    Matrix translation = MatrixTranslate(0, 0, 1);
    Matrix rotation = MatrixRotateXYZ((Vector3){PI * 2 - playerRotation.y, PI * 2 - playerRotation.x, 0});
    Matrix transform = MatrixMultiply(translation, rotation);

    camera->target.x = camera->position.x - transform.m12;
    camera->target.y = camera->position.y - transform.m13;
    camera->target.z = camera->position.z - transform.m14;
}

bool CheckBlockExposure(int blockX, int blockY, int blockZ, bool checkBelow, int index) {
	bool a = blockY < mapheights[index - 1];
	bool b = blockY < mapheights[index + 1];
	bool c = blockY < mapheights[index + (chunkSize * renderDistance)];
	bool d = blockY < mapheights[index - (chunkSize * renderDistance)];
	bool e = false;
	bool f = !(blockY == (mapheights[index]) - 1);

	if(checkBelow) {
		for(int y = blockY; y > 0; y--) {
			if(CheckBlockExposure(blockX, y, blockZ, false, index)) {
				e = true;
			} else {
				e = false;
				break;
			}
		}
	} else {
		return a && b && c && d && f;
	}

	return a && b && c && d && e && f;
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "minecraft clone");

    // Initialize the camera
    Camera3D camera = {0};
    camera.position = (Vector3){0.0f, 1.0f, 0.0f}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};   // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};       // Camera up vector (rotation towards target)
    camera.fovy = 80.0f;                           // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;        // Camera mode type

    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    // terrain and rendering info
    const int maxHeight = 10;

    for (int i = 0; i < chunkSize * (chunkSize * (pow(renderDistance, 2))); i++) // chunkSize * (chunkSize * (renderDistance^2)) 
    {
        //if (i == 0)
        //{
            mapheights[i] = 10 + rand() % 2;
        //}
        //else
        //{
            /*float num = rand() % 100;

            if (num < 50 && num > 10)
            {
                mapheights[i] = mapheights[i - 17] - (rand() % rand() % 2);
            }
            else if (num > 50 && num < 90)
            {
                mapheights[i] = mapheights[i - 17] + (rand() % rand() % 2);
            }
            else
            {
                mapheights[i] = mapheights[i - 17];
            }*/

            //mapheights[i] = rand() % mapheights[i - 1] + rand() % 5;
        //}

        if (mapheights[i] < 1)
        {
            mapheights[i] = 1;
        }
    }

	Ray ray = { 0 };

	RayCollision collision = { 0 };

    // tell raylib what fps to target
    SetTargetFPS(1000);

    // loop until window should close
    while (!WindowShouldClose())
    {
		if(IsKeyDown(KEY_RIGHT)) {
			selectedPosition++;
		}

		if(IsKeyDown(KEY_LEFT)) {
			selectedPosition--;
		}

		if(IsKeyDown(KEY_DOWN)) {
			mapheights[selectedPosition]--;
		}
    
        UpdateMouseMovement();

        // custom camera update
        UpdateGameCamera(&camera, &playerPosition);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        int i = 0;

        for (int x = 0; x < chunkSize * renderDistance; x++)
        {
            for (int z = 0; z < chunkSize * renderDistance; z++)
            {
                for (int y = 0; y < mapheights[i]; y++)
                {
					float cubeSize = 0.45f;
					
					// calculate cube position
					Vector3 cubePos = {
					    (float)(x - chunkSize / 2) * cubeSize,
					    (float)(y - chunkSize / 2) * cubeSize,
						(float)(z - chunkSize / 2) * cubeSize,
					};

					if(i == selectedPosition) {
						DrawCubeWires(cubePos, cubeSize, cubeSize, cubeSize, RED);
					}

                	// check if block should be rendered
					if(CheckBlockExposure(x, y, z, true, i)) {
						//DrawCubeWires(cubePos, cubeSize, cubeSize, cubeSize, RED);
					} else {
                    	// really fucking stupid shit to detect top layer of terrain
                    	if (y == (mapheights[i]) - 1)
                    	{
                        	Color color = GREEN;

                        	Vector3 colorVector = ColorToHSV(color);

                        	colorVector.z = calculateLightLevel(cubePos, camera.position, 20);

                        	Color newColor = ColorFromHSV(colorVector.x, colorVector.y, colorVector.z);

                        	DrawCube(cubePos, cubeSize, cubeSize, cubeSize, newColor);
                    	}
                    	else
                    	{
                        	Color color = DARKBROWN;

                        	Vector3 colorVector = ColorToHSV(color);

                        	colorVector.z = calculateLightLevel(cubePos, camera.position, 20);

                        	Color newColor = ColorFromHSV(colorVector.x, colorVector.y, colorVector.z);

                        	DrawCube(cubePos, 0.45, 0.45, 0.45, newColor);
                   		}
                    }
                }

                i++;
            }
        }

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }

    // close window and opengl context
    CloseWindow();
    return 0;
}
