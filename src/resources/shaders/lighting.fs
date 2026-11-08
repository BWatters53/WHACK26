#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

#define MAX_LIGHTS        4
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;

// Distance darkening
uniform float fadeDistance;    // distance at which brightness bottoms out
uniform float minBrightness;   // floor (0.0 = allow pure black)

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 tint = colDiffuse * fragColor;

    vec3 viewD = normalize(viewPos - fragPosition);

    // Normals from screen-space derivatives: works even if the GLB has none.
    // Gives a faceted look. Swap to normalize(fragNormal) if your mesh has good normals.
    vec3 normal = normalize(cross(dFdx(fragPosition), dFdy(fragPosition)));
    normal = faceforward(normal, -viewD, normal);   // always face the camera

    vec3 lightDot = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 light = vec3(0.0);

            if (lights[i].type == LIGHT_DIRECTIONAL)
                light = -normalize(lights[i].target - lights[i].position);

            if (lights[i].type == LIGHT_POINT)
                light = normalize(lights[i].position - fragPosition);

            float NdotL = max(dot(normal, light), 0.0);
            lightDot += lights[i].color.rgb * NdotL;

            float specCo = 0.0;
            if (NdotL > 0.0)
                specCo = pow(max(0.0, dot(viewD, reflect(-light, normal))), 16.0);
            specular += specCo;
        }
    }

    finalColor = texelColor * ((tint + vec4(specular, 1.0)) * vec4(lightDot, 1.0));
    finalColor += texelColor * (ambient / 10.0) * tint;

    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0 / 2.2));

    // Distance darkening, applied last so it affects every object
    float dist = length(viewPos - fragPosition);
    float brightness = clamp(1.0 - dist / fadeDistance, minBrightness, 1.0);
    finalColor.rgb *= brightness;
}
