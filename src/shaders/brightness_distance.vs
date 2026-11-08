#version 330

// Input vertex attributes (Raylib naming standards)
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec3 vertexNormal; // Added to prevent pipeline linking layout issues

// Input uniform matrices (Raylib exact system names)
uniform mat4 mvp;
uniform mat4 matModel;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragWorldPos;

void main()
{
    fragTexCoord = vertexTexCoord;

    // Replace pure black with white, preserving alpha
    fragColor = all(equal(vertexColor.rgb, vec3(0.0)))
        ? vec4(1.0, 1.0, 1.0, vertexColor.a)
        : vertexColor;

    // Calculate world-space vertex position using raylib's model matrix
    vec4 worldPosition = matModel * vec4(vertexPosition, 1.0);
    fragWorldPos = worldPosition.xyz;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
