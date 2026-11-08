/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include <stdio.h>
#include <stdlib.h>
#include "float.h"

RayCollision GetRayCollisionModel(Ray ray, Model model, Vector3 position)
{
    RayCollision closestCollision = { 0 };
    closestCollision.hit = false;
    closestCollision.distance = FLT_MAX;

    Matrix translation = MatrixTranslate(
        position.x, position.y, position.z
    );

    Matrix transform = MatrixMultiply(translation, model.transform);

    for (int i = 0; i < model.meshCount; i++)
    {
        RayCollision hit = GetRayCollisionMesh(
            ray,
            model.meshes[i],
            transform
        );

        if (hit.hit && hit.distance < closestCollision.distance)
        {
            closestCollision = hit;
        }
    }

    return closestCollision;
}

void ApplyShaderToModel(Model *model, Shader shader)
{
    for (int i = 0; i < model->materialCount; i++)
    {
        model->materials[i].shader = shader;
    }
}

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1000, 800, "Hello Raylib");
    SearchAndSetResourceDir("resources");

    // Dimensions Init
    char dimensions[4][11] = {"2D", "3D", "Hyperbolic", "4D?"};

    const float cameraHeight = 0.5f;
    Camera camera = { 0 };
    camera.position   = (Vector3){ 0.0f, cameraHeight, 10.0f };
    camera.target     = (Vector3){ 50.0f, 2.0f, 50.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Shader shader = LoadShader(
        "shaders/brightness_distance.vs",
        "shaders/brightness_distance.fs"
    );
    printf("Shader ID: %u\n", shader.id);
    printf("Raylib version: %s\n", RAYLIB_VERSION);

    if (shader.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load fog shader");
        CloseWindow();
        return 1;
    }

    shader.locs[SHADER_LOC_MATRIX_MVP] =
        GetShaderLocation(shader, "mvp");

    shader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocation(shader, "matModel");

    int cameraPosLoc = GetShaderLocation(shader, "cameraPos");
    int maxDistanceLoc = GetShaderLocation(shader, "maxDistance");

    float maxDistance = 40.0f;

    SetShaderValue(
        shader,
        maxDistanceLoc,
        &maxDistance,
        SHADER_UNIFORM_FLOAT
    );
    printf("MVP location: %d\n",
        shader.locs[SHADER_LOC_MATRIX_MVP]);

    printf("Model location: %d\n",
        shader.locs[SHADER_LOC_MATRIX_MODEL]);

    printf("Camera location: %d\n", cameraPosLoc);

    printf("Max distance location: %d\n", maxDistanceLoc);
    Model floor = LoadModel("../assets/groundOnly.glb");
    Vector3 floor_pos = { 0.0f, -30.0f, 0.0f };

    Model map_objects = LoadModel("../assets/stuffOnGround.glb");
    Vector3 map_objects_pos = { 0.0f, -30.0f, 0.0f };

    for (int i = 0; i < floor.materialCount; i++)    floor.materials[i].shader    = shader;
    for (int i = 0; i < map_objects.materialCount; i++) map_objects.materials[i].shader = shader;

    const float turnSpeed = 90.0f * DEG2RAD;   // radians per second
    const float moveSpeed = 20.0f;              // world units per second

    int currentDimension = 0;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Turn: +1 for left, -1 for right
        float turn = 0.0f;
        if (IsKeyDown(KEY_LEFT))  turn += 1.0f;
        if (IsKeyDown(KEY_RIGHT)) turn -= 1.0f;

        if (turn != 0.0f)
        {
            Vector3 forward = Vector3Subtract(camera.target, camera.position);
            forward = Vector3RotateByAxisAngle(forward, camera.up, turn * turnSpeed * dt);
            camera.target = Vector3Add(camera.position, forward);
        }
        // Tilt: +1 for up, -1 for down
        float tilt = 0.0f;
        if (IsKeyDown(KEY_S)) tilt += 1.0f;
        if (IsKeyDown(KEY_W)) tilt -= 1.0f;

        if (tilt != 0.0f)
        {
            Vector3 forward = Vector3Subtract(camera.target, camera.position);
            Vector3 rotationAxis = Vector3CrossProduct(forward, camera.up);
            forward = Vector3RotateByAxisAngle(forward, rotationAxis, tilt * turnSpeed * dt);
            camera.target = Vector3Add(camera.position, forward);
        }

        // Move: +1 forward, -1 backward
        float move = 0.0f;
        if (IsKeyDown(KEY_UP))   move += 1.0f;
        if (IsKeyDown(KEY_DOWN)) move -= 1.0f;

        if (move != 0.0f)
        {
            Vector3 forward = {
                camera.target.x - camera.position.x,
                0.0f,
                camera.target.z - camera.position.z
            };

            Vector3 step = Vector3Normalize(forward);
            step = Vector3Scale(step, moveSpeed * move * dt);

            camera.position = Vector3Add(camera.position, step);
            camera.target = Vector3Add(camera.target, step);
        }

        if (IsKeyPressed(KEY_E)) {
            currentDimension = (currentDimension + 1)%(sizeof(dimensions)/sizeof(dimensions[0]));
        }

        // Keep the camera's viewing direction
        Vector3 targetOffset = Vector3Subtract(
            camera.target,
            camera.position
        );

        // Cast ray downwards
        Ray ray = {
            camera.position,
            (Vector3){ 0.0f, -1.0f, 0.0f }
        };

        RayCollision collision = GetRayCollisionModel(ray, floor, floor_pos);

        if (collision.hit)
        {
            const float playerHeight = cameraHeight;
            float desiredHeight = collision.point.y + playerHeight;
            camera.position.y = desiredHeight;
            camera.target = Vector3Add(camera.position, targetOffset);
        }

    SetShaderValue(shader, cameraPosLoc, &camera.position, SHADER_UNIFORM_VEC3);


        BeginDrawing();
        ClearBackground(BLACK); 

        BeginMode3D(camera);
            
            // FORCE raylib to override all materials and use your shader globally
            BeginShaderMode(shader);

                DrawModel(floor, floor_pos, 1.0f, WHITE);
                DrawModel(map_objects, map_objects_pos, 1.0f, RED);

            EndShaderMode(); // Stop using the shader

            DrawGrid(10, 1.0f); // Draw the grid normally

        EndMode3D();

            
        // Setting Box
        DrawRectangle(0, 0, 250, 30, ColorAlpha(BLACK, 0.5f));
        char *currentDimensionString = (char*)malloc(13 * sizeof(char));
        sprintf(currentDimensionString, "%s mode active", dimensions[currentDimension]);
        DrawText(currentDimensionString,5,5,20,WHITE);
        free(currentDimensionString); 

        EndDrawing();
    }

    UnloadModel(floor);
    UnloadModel(map_objects);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
