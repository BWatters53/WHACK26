#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "float.h"
#include "rlgl.h"

enum DIMENSION {TWO = 2, THREE = 3};
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

// Set to 1 to keep the point light on the camera (fog and lighting then agree),
// 0 to leave it fixed in the world at its starting position.
#define LIGHT_FOLLOWS_CAMERA 1

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

// Just For Testing Purposes (-:
int printLocation(Vector3 position) {
    printf("Camera Position: x: %f, y: %f, z: %f\n", position.x, position.y, position.z);
    return 0;
}

int main(void)
{
    bool isHyperbolic = false;
    const float MAP_BOUNDS = 50.0f;
    const int zero = 0;
    const int one = 1;
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1000, 800, "Hello Raylib");
    SearchAndSetResourceDir("src/resources");

	// Dimensions Init
	char dimensions[3][11] = {"2D", "3D", "Hyperbolic"};

    const float cameraHeight = 1.5f;
    Camera camera = { 0 };
    camera.position   = (Vector3){ 0.0f, 2.0f, 10.0f };
    camera.target     = (Vector3){ 50.0f, 2.0f, 50.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    Matrix defaultViewMat = GetCameraMatrix(camera);

    // Load the lighting shader (paths are relative to the resources directory)
    Shader shader = LoadShader("shaders/lighting.vs", "shaders/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    if (shader.id != 0 ) {
        printf("ShaderID:%d",shader.id);
        printf("shader id = %u, default id = %u\n", shader.id, rlGetShaderIdDefault());
    }

    // The shader divides this by 10 internally; lower it for darker distant areas
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.0f, 0.0f, 0.0f, 1.0f }, SHADER_UNIFORM_VEC4);

    float fadeDistance  = 80.0f;   // units until fully dark
    float minBrightness = 0.05f;   // 0.0 = pure black at the limit

    int fadeDistanceLoc  = GetShaderLocation(shader, "fadeDistance");
    int minBrightnessLoc = GetShaderLocation(shader, "minBrightness");

    SetShaderValue(shader, fadeDistanceLoc,  &fadeDistance,  SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, minBrightnessLoc, &minBrightness, SHADER_UNIFORM_FLOAT);

    int cameraPosUniform = GetShaderLocation(shader, "cameraPos");
    int doHyperbolic = GetShaderLocation(shader, "doHyperbolic");
    float pontiDiskRad = GetShaderLocation(shader, "pontiDiskRadSqrd");
    float pontiDiskRadius = MAP_BOUNDS * MAP_BOUNDS;
    SetShaderValue(shader, doHyperbolic, &zero, SHADER_UNIFORM_INT);
    SetShaderValue(shader, pontiDiskRad, &pontiDiskRadius, SHADER_UNIFORM_FLOAT);

    // Point light
    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ 25.0f, 25.0f, 25.0f }, Vector3Zero(), WHITE, shader);

    Model floor = LoadModel("../../assets/groundOnly.glb");
    Vector3 floor_pos = { 0.0f, -30.0f, 0.0f };

    Model floor_objects = LoadModel("../../assets/stuffOnGround.glb");
    Vector3 floor_objects_pos = { 0.0f, -30.0f, 0.0f };

	// 2D Variable Init
	int widthOf2D = (int) GetScreenHeight() / 50;
	RenderTexture2D target = LoadRenderTexture(GetScreenWidth(),GetScreenHeight());

    // DrawModel uses each material's own shader, NOT the one from BeginShaderMode,
    // so the lighting shader has to be assigned to every material on every model.
    for (int i = 0; i < floor.materialCount; i++)    floor.materials[i].shader    = shader;
    for (int i = 0; i < floor_objects.materialCount; i++) floor_objects.materials[i].shader = shader;

    const float turnSpeed = 90.0f * DEG2RAD;   // radians per second
    const float moveSpeed = 20.0f;             // world units per second

	int currentDimension = 1;

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
        if (!(strcmp(dimensions[currentDimension], "2D") == 0)) { //Can't Rotate In 3D Space When In 2D Mode.
            if (IsKeyDown(KEY_S)) tilt += 1.0f;
            if (IsKeyDown(KEY_W)) tilt -= 1.0f;
        }
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
            if (strcmp(dimensions[currentDimension], "3D") == 0){
                UnloadRenderTexture(target);
                isHyperbolic = false;
            } else if (strcmp(dimensions[currentDimension], "2D") == 0) {
                target = LoadRenderTexture(GetScreenWidth(),GetScreenHeight());
                ClearBackground(SKYBLUE);
                isHyperbolic = false;

            } else if (strcmp(dimensions[currentDimension], "Hyperbolic") == 0){
                isHyperbolic = true;
            }
        }

        if (IsKeyPressed(KEY_R)) {
            Matrix currentViewMat = GetCameraMatrix(camera);
            printf("Player Pos: %f, %f, %f \n", camera.position.x, camera.position.y, camera.position.z);
        }

        SetShaderValue(shader, fadeDistanceLoc, &fadeDistance, SHADER_UNIFORM_FLOAT);

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
            const float playerHeight = 2.0f;

            float desiredHeight = collision.point.y + playerHeight;

            // Move camera to maintain distance from terrain
            camera.position.y = desiredHeight;

            // Preserve viewing direction
            camera.target = Vector3Add(
                camera.position,
                targetOffset
            );
        }

        #if LIGHT_FOLLOWS_CAMERA
                lights[0].position = camera.position;
        #endif

        if (isHyperbolic){
            SetShaderValue(shader, doHyperbolic, &one, SHADER_UNIFORM_INT);    // C-style bool
            SetShaderValue(shader, cameraPosUniform, (float[4]){ camera.position.x, camera.position.y, camera.position.z, 0.0f }, SHADER_UNIFORM_VEC4);  // Set uniform
        }
        else{
            SetShaderValue(shader, doHyperbolic, &zero, SHADER_UNIFORM_INT);    // C-style bool
        }

        // Push the light values to the shader
        UpdateLightValues(shader, lights[0]);

        // The shader needs the camera position for specular highlights AND fog distance
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW],
                       &camera.position.x, SHADER_UNIFORM_VEC3);
		if (strcmp(dimensions[currentDimension], "2D") == 0) {
			BeginDrawing();
			BeginTextureMode(target);
		}
        ClearBackground(SKYBLUE);
        BeginMode3D(camera);
		
		
        DrawModel(floor, floor_pos, 1.0f, WHITE);
        DrawModel(floor_objects, floor_objects_pos, 1.0f, RED);

        // Light marker (hidden while the light sits exactly on the camera)
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
            else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
        }
		EndMode3D();
		if (strcmp(dimensions[currentDimension], "2D") == 0) {
			EndTextureMode();
			int y = GetScreenHeight()/2-widthOf2D;
			for (y;y<GetScreenHeight()/2+widthOf2D;y++) {
				DrawTextureRec(target.texture, (Rectangle){ 0, -(GetScreenHeight()/2), GetScreenWidth(),  -1}, (Vector2){ 0, y }, WHITE);
			}
		}

		//DrawFPS(10, 10); // Do we need this?
		
		// 2D Blocker
		if (strcmp(dimensions[currentDimension], "2D") == 0) {
			DrawRectangle(0,0, GetScreenWidth(),GetScreenHeight()/2-widthOf2D,BLACK);
			DrawRectangle(0,GetScreenHeight()/2+widthOf2D,GetScreenWidth(),GetScreenHeight()/2-widthOf2D,BLACK);
		}

		// Setting Box
		DrawRectangle(0, 0, 500, 30, ColorAlpha(BLACK, 0.5f));
		char *currentDimensionString = (char*)malloc(45 * sizeof(char));
		sprintf(currentDimensionString, "%s Mode Active. Press [E] To Change", dimensions[currentDimension]);
		DrawText(currentDimensionString,5,5,20,WHITE);

        EndDrawing();
    }

    UnloadModel(floor);
    UnloadModel(floor_objects);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
