#pragma once

static const char* GLSL_BASIC_VERT = R"(#version 330
in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 matModel;
uniform mat4 mvp;
uniform mat4 lightSpaceMatrix;

out vec4 fragPosLightSpace;
out vec3 fragNormal;
out vec3 fragPos;

void main() {
    fragPos = vec3(matModel * vec4(vertexPosition, 1.0));
    fragNormal = normalize(mat3(matModel) * vertexNormal);

    gl_Position = mvp * vec4(vertexPosition, 1.0);
})";

static const char* GLSL_BASIC_FRAG = R"(#version 330

in vec3 fragNormal;
in vec3 fragPos;
in vec4 fragPosLightSpace;

uniform sampler2D shadowMap;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 albedoColor;

out vec4 finalColor;

float ShadowFactor() {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // outside shadow map = no shadow
    if (projCoords.z > 1.0) return 1.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.002;
    return currentDepth - bias > closestDepth ? 0.3 : 1.0;
}

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(-lightDir);

    float diff = max(dot(N, L), 0.0);

    float shadow = ShadowFactor();

    vec3 lighting =
        ambientColor +
        diff * lightColor * shadow;

    vec3 color = albedoColor * lighting;

    finalColor = vec4(color, 1.0);
})";

static const char* GLSL_TEXTURE_VERT = R"(#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform vec3 partSize;      // full size of the part (width, height, depth)
uniform vec2 tileCount;     // tiles per face

out vec2 fragTexCoord;

void main()
{
    vec2 uv;
    vec2 scaleAxes;

    vec3 absNormal = abs(vertexNormal);

    if (absNormal.x > 0.5) {
        uv = vertexPosition.yz;
        scaleAxes = vec2(partSize.y, partSize.z);  // use Y & Z
    }
    else if (absNormal.y > 0.5) {
        uv = vertexPosition.xz;
        scaleAxes = vec2(partSize.x, partSize.z);  // use X & Z
    }
    else {
        uv = vertexPosition.xy;
        scaleAxes = vec2(partSize.x, partSize.y);  // use X & Y
    }

    // normalize by face size along those axes
    uv /= scaleAxes;

    // tile the texture
    uv *= tileCount;

    fragTexCoord = uv;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
})";

static const char* GLSL_TEXTURE_FRAG = R"(#version 330

in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D texture0;

out vec4 finalColor;

void main() {
    vec4 texColor = texture(texture0, fragTexCoord);

    // if(texColor.a < 0.1)
        // discard;

    finalColor = texColor;
})";

static const char* GLSL_DEPTH_FRAG = R"(#version 330
void main() { })";