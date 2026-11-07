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

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(800, 600, "Hello Raylib");
    SearchAndSetResourceDir("resources");

    // Not looking straight down anymore, so "up" isn't parallel to the view direction
    Camera camera = { 0 };
    camera.position   = (Vector3){ 0.0f, 2.0f, 10.0f };
    camera.target     = (Vector3){ 50.0f, 0.0f, 50.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Texture wabbit = LoadTexture("wabbit_alpha.png");
    Model floor = LoadModel("../assets/ground.glb");
    Vector3 floor_pos = { 0.0f, 0.0f, 0.0f };

    const float turnSpeed = 90.0f * DEG2RAD;   // radians per second

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

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Hello Raylib", 200, 200, 20, WHITE);

        BeginMode3D(camera);
        DrawModel(floor, floor_pos, 1.0f, WHITE);
        EndMode3D();

        EndDrawing();
    }

    UnloadTexture(wabbit);
    UnloadModel(floor);
    CloseWindow();
    return 0;
}
