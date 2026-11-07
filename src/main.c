/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"        // Vector3Subtract, Vector3Add, Vector3RotateByAxisAngle
#include "resource_dir.h"
#include <stdio.h>
#include <stdlib.h>

enum DIMENSION {TWO= 2, THREE= 3};

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(800, 600, "Hello Raylib");
    SearchAndSetResourceDir("resources");

    // Not looking straight down anymore, so "up" isn't parallel to the view direction
    Camera camera = { 0 };
    camera.position   = (Vector3){ 0.0f, 2.0f, 10.0f };
    camera.target     = (Vector3){ 50.0f, 2.0f, 50.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model floor = LoadModel("../assets/ground.glb");
    Vector3 floor_pos = { 0.0f, 0.0f, 0.0f };

    Model red_cube = LoadModel("../assets/redCube.glb");
    Vector3 red_cube_pos = { 0.0f, 1.0f, 0.0f };

    const float turnSpeed = 90.0f * DEG2RAD;   // radians per second

	enum DIMENSION currentDimension = THREE;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // +1 for left, -1 for right, 0 for neither/both
        float turn = 0.0f;
        if (IsKeyDown(KEY_LEFT))  turn += 1.0f;
        if (IsKeyDown(KEY_RIGHT)) turn -= 1.0f;

        if (turn != 0.0f)
        {
            // View direction relative to the camera, not the world origin
            Vector3 forward = Vector3Subtract(camera.target, camera.position);

            // Rotate around the camera's up axis (yaw)
            forward = Vector3RotateByAxisAngle(forward, camera.up, turn * turnSpeed * dt);

            // Move the target back out from the camera position
            camera.target = Vector3Add(camera.position, forward);
        }

        // +1 for forward, -1 for backward, 0 for neither/both
        float move = 0.0f;
        if (IsKeyDown(KEY_UP)) move += 1.0f;
        if (IsKeyDown(KEY_DOWN)) move -= 1.0f;

        if (move != 0.0f)
        {
            // View direction relative to the camera, not the world origin
            Vector3 forward = Vector3Subtract(camera.target, camera.position);

            forward = Vector3Scale(forward, 1.0f * move * dt);

            camera.position = Vector3Add(camera.position, forward);

            // Move the target back out from the camera position
            camera.target = Vector3Add(camera.target, forward);

        }

		if (IsKeyPressed(KEY_E)) {
			if (currentDimension == THREE) {
				currentDimension = TWO;
			} else {
				currentDimension = THREE;
			}
		}

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the models

        BeginMode3D(camera);

        DrawModel(floor, floor_pos, 1.0f, WHITE);
        DrawModel(red_cube, red_cube_pos, 1.0f, RED);

        EndMode3D();
		
		char *currentDimensionString = (char*)malloc(13 * sizeof(char));
		sprintf(currentDimensionString, "%dD mode active", currentDimension);
		DrawText(currentDimensionString,100,100,20,WHITE);

        EndDrawing();
    }

    UnloadModel(floor);
    CloseWindow();
    return 0;
}
