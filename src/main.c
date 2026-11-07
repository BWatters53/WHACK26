/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "math.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include <math.h>
#include <stdio.h>

Vector3 transformVector3(Vector3 vec, double mat[3][3]) 
{
	Vector3 outputVector = (Vector3) {vec.x*mat[0][0] + vec.y*mat[0][1] + vec.z*mat[0][2], vec.x*mat[1][0] + vec.y*mat[1][1] + vec.z*mat[1][2],vec.x*mat[2][0] + vec.y*mat[2][1] + vec.z*mat[2][2]};
	return outputVector;
}

int main ()
{    
	Camera camera = { 0 };
	camera.position = (Vector3){ 50.0f, 50.0f, 50.0f }; // Camera position
	camera.target = (Vector3){ 0.0f, 12.0f, 0.0f };     // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type


	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(800, 600, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	Model floor = LoadModel("../assets/ground.glb");

	Vector3 floor_pos = { 0.0f, 0.0f, 0.0f };

	
	int counter = 0;
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		if (IsKeyDown(KEY_RIGHT)) {
			double transformation_mat[3][3] = {{cos(20.0*(PI/180.0)), 0.0, sin(20.0*(PI/180.0))},{0.0,1.0,0.0},{-sin(20.0*(PI/180.0)),0.0,cos(20.0*(PI/180.0))}};
			printf("i am printing a %d\n",transformation_mat[0][0]);
			printf("atnoehusantoehusnth%d\n",counter++);
			camera.target = transformVector3(camera.target,transformation_mat);
		}
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText("Hello Raylib", 200,200,20,WHITE);

		BeginMode3D(camera);
		
		DrawModel(floor, floor_pos, 1.0f, WHITE);

		EndMode3D();
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();

		
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

