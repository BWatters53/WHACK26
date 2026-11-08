#version 330

// Input attributes automatically provided by raylib's mesh data
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Transform matrices automatically updated by raylib's 3D engine
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

// Output variables sent to our fragment shader
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec4 fragColor;

void main()
{
    // Calculate the true world-space position of this vertex
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    
    // Pass along basic attributes
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    
    // Transform and normalize the geometric surface normal
    fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));
    
    // Standard projection coordinate for final screen positioning
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
