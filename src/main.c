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

enum DIMENSION {TWO= 2, THREE= 3};
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1000, 800, "Hello Raylib");
    SearchAndSetResourceDir("resources");

    Camera camera = { 0 };
    camera.position   = (Vector3){ 0.0f, 2.0f, 10.0f };
    camera.target     = (Vector3){ 50.0f, 2.0f, 50.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Load the lighting shader (paths are relative to the resources directory)
    Shader shader = LoadShader("shaders/lighting.vs", "shaders/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    // The shader divides this by 10 internally; lower it for darker distant areas
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.0f, 0.0f, 0.0f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Point light that follows the camera
    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){25.0f,25.0f,25.0f}, Vector3Zero(), WHITE, shader);

    Model floor = LoadModel("../assets/ground.glb");
    Vector3 floor_pos = { 0.0f, 0.0f, 0.0f };

    Model red_cube = LoadModel("../assets/tree2.glb");
    Vector3 red_cube_pos = { 0.0f, 1.0f, 0.0f };

    // DrawModel uses each material's own shader, NOT the one from BeginShaderMode,
    // so the lighting shader has to be assigned to every material on every model.
    for (int i = 0; i < floor.materialCount; i++)    floor.materials[i].shader    = shader;
    for (int i = 0; i < red_cube.materialCount; i++) red_cube.materials[i].shader = shader;

    const float turnSpeed = 90.0f * DEG2RAD;   // radians per second
    const float moveSpeed = 5.0f;              // world units per second

	enum DIMENSION currentDimension = THREE;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Turn: +1 for left, -1 for right
        float turn = 0.0f;
        if (IsKeyDown(KEY_LEFT))  turn += 1.0f;
        if (IsKeyDown(KEY_RIGHT)) turn -= 1.0f;

        if (turn != 0.0f)
        {
            // View direction relative to the camera, not the world origin
            Vector3 forward = Vector3Subtract(camera.target, camera.position);

            // Yaw around the camera's up axis
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
            Vector3 step = Vector3Normalize(Vector3Subtract((Vector3){camera.target.x,0.0f,camera.target.z}, camera.position));
            step = Vector3Scale(step, moveSpeed * move * dt);

            camera.position = Vector3Add(camera.position, step);
            camera.target   = Vector3Add(camera.target, step);
        }

	if (IsKeyPressed(KEY_E)) {
		if (currentDimension == THREE) {
			currentDimension = TWO;
		} else {
			currentDimension = THREE;
		}
	}

        // Keep the light on the camera and push the new values to the shader
        //lights[0].position = camera.position;
        UpdateLightValues(shader, lights[0]);

        // The shader needs the camera position for specular highlights
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW],
                       &camera.position.x, SHADER_UNIFORM_VEC3);

        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode3D(camera);

        DrawModel(floor, floor_pos, 1.0f, WHITE);
        DrawModel(red_cube, red_cube_pos, 1.0f, RED);

        // Light marker (hidden while the light sits exactly on the camera)
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
            else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
        }

        EndMode3D();
		
		char *currentDimensionString = (char*)malloc(13 * sizeof(char));
		sprintf(currentDimensionString, "%dD mode active", currentDimension);
		DrawText(currentDimensionString,100,100,20,WHITE);


        EndDrawing();
    }

    UnloadModel(floor);
    UnloadModel(red_cube);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
