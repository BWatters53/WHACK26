#version 330

// Input vertex attributes (From vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragWorldPos;

// Uniforms requested by your C code application
uniform vec3 cameraPos;
uniform float maxDistance;

// Output color
out vec4 finalColor;

void main()
{
    // Your magenta test layout
    finalColor = vec4(1.0, 0.0, 1.0, 1.0);
    
    // Prevent the compiler from optimizing out uniforms you set in C
    // (GPUs strip unused uniforms, breaking GetShaderLocation)
    float distanceTest = distance(cameraPos, fragWorldPos);
    if (distanceTest > maxDistance) {
        // Just a dummy check using the variables to force compilation retention
        finalColor.rgb *= 1.0; 
    }
}
