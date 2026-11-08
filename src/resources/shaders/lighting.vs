#version 330

// Input vertex attributes (raylib default names)
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniforms (raylib default names)
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

// Special uniforms needed for hyperbolic stuff (when non-hyper this is set to 0, 0, 0)
uniform int doHyperbolic;
uniform float pontiDiskRadSqrd;
uniform vec4 cameraPos;

// Output to fragment shader
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

void main()
{
    // World-space position, used for lighting and fog distance
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor    = vertexColor;
    fragNormal   = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));

    if (doHyperbolic != 0){
        vec4 globalVertexPosition = matModel * vec4(vertexPosition, 1.0);
        vec3 cameraVertexDist = vec3(cameraPos.x - globalVertexPosition.x, cameraPos.y - globalVertexPosition.y, cameraPos.z - globalVertexPosition.z);
        float cameraVertexDistSqrd = cameraVertexDist.x * cameraVertexDist.x + cameraVertexDist.y * cameraVertexDist.y + cameraVertexDist.z * cameraVertexDist.z;
        if (cameraVertexDistSqrd <= 0.1f) { cameraVertexDistSqrd == 0.1f; }
        
        float hyperbolicMultiplier = (1.0f/cameraVertexDistSqrd) * pontiDiskRadSqrd;
        mat4 scalingMatrix = mat4(
            hyperbolicMultiplier, 0.0,      0.0,      0.0,  // Column 0
            0.0,      hyperbolicMultiplier, 0.0,      0.0,  
            0.0,      0.0,      hyperbolicMultiplier, 0.0,  
            0.0,      0.0,      0.0,      1.0             
        );


        //mat4 stretch = mat4(
            //cameraVertexDist.x, 0.0,      0.0,      0.0,  // Column 0
            //0.0,      cameraVertexDist.y, 0.0,      0.0,  
            //0.0,      0.0,      cameraVertexDist.z, 0.0,  
           // 0.0,      0.0,      0.0,      1.0             
        //);
        //scalingMatrix = stretch * scalingMatrix;

        gl_Position = mvp * inverse(matModel) * scalingMatrix * vec4(-cameraVertexDist, 1.0);
    }
    else{
        gl_Position = mvp * vec4(vertexPosition, 1.0);
    }
}
