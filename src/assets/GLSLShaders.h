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

out vec4 finalColor;

float ShadowFactor() {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; // map to [0,1]

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float bias = 0.002;
	return currentDepth - bias > closestDepth ? 0.3 : 1.0;
}

void main() {
	vec3 N = normalize(fragNormal);
	vec3 L = normalize(-lightDir);

	float diff = max(dot(N, L), 0.0);
	vec3 color = diff * lightColor + ambientColor;

	vec3 n = normalize(N);
	float edge = length(fwidth(n));

	edge = smoothstep(0.02, 0.08, edge);

	finalColor = vec4(color * ShadowFactor(), 1.0);
})";

static const char* GLSL_LIT_VERT = R"(#version 330
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 lightVP0;
uniform mat4 lightVP1;
uniform mat4 lightVP2;

uniform float normalBiasWS0;
uniform float normalBiasWS1;
uniform float normalBiasWS2;

out vec3 vN;
out vec3 vWPos;
out vec2 vTexCoord;
out vec4 vLS0;
out vec4 vLS1;
out vec4 vLS2;

void main(){
    mat3 nmat = mat3(transpose(inverse(matModel)));
    vN = normalize(nmat * vertexNormal);
    vec4 worldPos = matModel * vec4(vertexPosition,1.0);
    vWPos = worldPos.xyz;
    vTexCoord = vertexTexCoord;

    // apply normal-space small offset per-cascade to avoid contact gaps
    vec3 Nw = normalize(vN);
    vec4 worldPos0 = worldPos + vec4(Nw * normalBiasWS0, 0.0);
    vec4 worldPos1 = worldPos + vec4(Nw * normalBiasWS1, 0.0);
    vec4 worldPos2 = worldPos + vec4(Nw * normalBiasWS2, 0.0);

    vLS0 = lightVP0 * worldPos0;
    vLS1 = lightVP1 * worldPos1;
    vLS2 = lightVP2 * worldPos2;

    gl_Position = mvp * vec4(vertexPosition,1.0);
})";

static const char* GLSL_LIT_INST_VERT = R"(#version 330
in vec3 vertexPosition;
in vec3 vertexNormal;

// Per-instance model matrix provided as vertex attribute
in mat4 instanceTransform;

uniform mat4 mvp;
uniform mat4 lightVP0;
uniform mat4 lightVP1;
uniform mat4 lightVP2;

uniform float normalBiasWS0;
uniform float normalBiasWS1;
uniform float normalBiasWS2;

out vec3 vN;
out vec3 vWPos;
out vec4 vLS0;
out vec4 vLS1;
out vec4 vLS2;

void main() {
    mat3 nmat = mat3(transpose(inverse(instanceTransform)));
    vN = normalize(nmat * vertexNormal);

    vec4 worldPos = instanceTransform * vec4(vertexPosition,1.0);
    vWPos = worldPos.xyz;

    vec3 Nw = normalize(vN);
    vec4 worldPos0 = worldPos + vec4(Nw * normalBiasWS0, 0.0);
    vec4 worldPos1 = worldPos + vec4(Nw * normalBiasWS1, 0.0);
    vec4 worldPos2 = worldPos + vec4(Nw * normalBiasWS2, 0.0);

    vLS0 = lightVP0 * worldPos0;
    vLS1 = lightVP1 * worldPos1;
    vLS2 = lightVP2 * worldPos2;

    gl_Position = mvp * worldPos;
})";

static const char* GLSL_LIT_FRAG = R"(#version 330
in vec3 vN;
in vec3 vWPos;
in vec2 vTexCoord;
in vec4 vLS0;
in vec4 vLS1;
in vec4 vLS2;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 NormalColor; // For Normal G-Buffer

uniform vec3 viewPos;

// Lighting
uniform vec3 sunDir;
uniform vec3 skyColor;
uniform vec3 groundColor;
uniform float hemiStrength;
uniform float sunStrength;
uniform vec3 ambientColor;

// Spec/Fresnel
uniform float specStrength;
uniform float shininess;
uniform float fresnelStrength;

// AO
uniform float aoStrength;
uniform float groundY;

// CSM
uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform int shadowMapResolution;
uniform float pcfStep;
uniform float biasMin;
uniform float biasMax;
uniform vec3 cascadeSplits;
uniform float transitionFrac;

// exposure
uniform float exposure;

// Material
uniform vec4 colDiffuse;
uniform sampler2D texture0;

// -----------------------------------------
float SampleShadow(sampler2D smap, vec3 proj, float ndl){
    float bias = mix(biasMax, biasMin, ndl);
    float step = pcfStep / float(shadowMapResolution);
    float sum = 0.0;
    for(int x=-1;x<=1;x++)
    for(int y=-1;y<=1;y++){
        vec2 off = vec2(x,y) * step;
        float depth = texture(smap, proj.xy + off).r;
        sum += (proj.z - bias <= depth) ? 1.0 : 0.0;
    }
    return sum / 9.0;
}


float ShadowForCascade(int idx, vec3 proj, float ndl){
    if(proj.x<0.0||proj.x>1.0||proj.y<0.0||proj.y>1.0||proj.z<0.0||proj.z>1.0)
        return 1.0;
    if(idx==0) return SampleShadow(shadowMap0, proj, ndl);
    if(idx==1) return SampleShadow(shadowMap1, proj, ndl);
    return SampleShadow(shadowMap2, proj, ndl);
}

float ShadowBlend(vec3 p0, vec3 p1, vec3 p2, float ndl, float viewDepth, vec3 splits){
    float d0 = splits.x;
    float d1 = splits.y;
    float frac = clamp(transitionFrac,0.0,0.5);
    float band0 = max(0.001,d0*frac);
    float band1 = max(0.001,d1*frac);

    if(viewDepth <= d0 - band0) return ShadowForCascade(0,p0,ndl);
    if(viewDepth < d0 + band0){
        float t = smoothstep(d0-band0,d0+band0,viewDepth);
        return mix(ShadowForCascade(0,p0,ndl), ShadowForCascade(1,p1,ndl), t);
    }
    if(viewDepth <= d1 - band1) return ShadowForCascade(1,p1,ndl);
    if(viewDepth < d1 + band1){
        float t = smoothstep(d1-band1,d1+band1,viewDepth);
        return mix(ShadowForCascade(1,p1,ndl), ShadowForCascade(2,p2,ndl), t);
    }
    return ShadowForCascade(2,p2,ndl);
}

// -----------------------------------------
void main(){
    vec3 N = normalize(vN);
    vec3 L = normalize(-sunDir);
    vec3 V = normalize(viewPos - vWPos);
    vec3 H = normalize(L+V);

    // Hemispheric lighting
    float t = clamp(N.y*0.5 + 0.5, 0.0, 1.0);
    vec3 hemi = mix(groundColor, skyColor, t) * hemiStrength;

    // Diffuse & Specular
    float ndl = max(dot(N,L),0.0);
    float spec = pow(max(dot(N,H),0.0), shininess) * specStrength * 0.1;
    float FH = pow(1.0 - max(dot(N,V),0.0), 5.0);
    float fresnel = mix(0.02,1.0,FH) * fresnelStrength;

    // AO
    float height = max(vWPos.y - groundY, 0.0);
    float aoGround = clamp(1.0 - exp(-height*0.15),0.3,1.0);
    float ao = mix(1.0, aoGround, aoStrength);

    // Shadow
    float viewDepth = length(viewPos - vWPos);
    vec3 p0 = vLS0.xyz / vLS0.w * 0.5 + 0.5;
    vec3 p1 = vLS1.xyz / vLS1.w * 0.5 + 0.5;
    vec3 p2 = vLS2.xyz / vLS2.w * 0.5 + 0.5;
    float shadow = ShadowBlend(p0,p1,p2,ndl,viewDepth,cascadeSplits);

    // Texture
    vec4 texColor = texture(texture0,vTexCoord);
    vec3 base = pow((texColor * colDiffuse).rgb, vec3(2.2));

    // Lighting
    float sunTerm = sunStrength * ndl * shadow;
    spec *= shadow;
    fresnel *= shadow;
    vec3 color = base*(hemi*ao + sunTerm) + spec + fresnel;
    color += base*ambientColor;
    
    // Note: exposure/tonemapping is now done in the final composite shader
    // We output linear, high-dynamic-range color here.
    FragColor = vec4(color, colDiffuse.a);

    // Output world-space normal to the second render target
    NormalColor = vec4(N * 0.5 + 0.5, 1.0); // Encode to [0,1] range
})";

static const char* GLSL_DEPTH_VERT = R"(#version 330
in vec3 vertexPosition;

uniform mat4 matModel;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = lightSpaceMatrix * matModel * vec4(vertexPosition, 1.0);
}})";

static const char* GLSL_DEPTH_FRAG = R"(#version 330
void main() { })";