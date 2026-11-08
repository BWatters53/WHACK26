#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragColor;

uniform vec4 colDiffuse; 
uniform vec3 cameraPos;
uniform vec4 fogColor;
uniform float fogDensity;

out vec4 finalColor;

void main()
{
    // Directional light math (will now correctly vary across the curve of the torus)
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float lightIntensity = max(dot(normalize(fragNormal), lightDir), 0.1); 
    vec4 baseColor = vec4(colDiffuse.rgb * lightIntensity, colDiffuse.a);

    // Fog depth calculation (will now correctly vary per-pixel across the object)
    float dist = length(cameraPos - fragPosition);
    float fogFactor = exp(-pow(dist * fogDensity, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    finalColor = mix(fogColor, baseColor, fogFactor);
}
