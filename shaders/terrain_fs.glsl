#version 430

layout (binding=0) uniform sampler2D heightMap;
layout (binding=1) uniform sampler2D normalMap;
layout (binding=2) uniform sampler2D gridTexture;
layout (binding=3) uniform sampler2D shadowMap;

uniform vec3 qt_eyeVector;

in vec2 mapCoords_FS;
in vec4 shadowCoords_FS;

in vec3 vertexNormal_FS;
in vec3 lightDirection_FS;

in vec4 worldPos_FS;

out vec4 fragColor;

float diffuse(vec3 direction, vec3 normal, float intensity)
{
    return max(0.04, dot(-direction, normal)) * intensity;
}

float random(vec2 seed)
{
    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

const float intensity = 1.0;
const int pcfCount = 2;
const float totalTexels = (pcfCount * 2.0 + 1.0) * (pcfCount * 2.0 + 1.0);

void main()
{

    const float shadowBias = 0.0; // don't use, it shifts the shadow
    vec3 lightCoords = shadowCoords_FS.xyz;

    float mapSize = textureSize(shadowMap, 0).x;
    float texelSize = 1.0 / mapSize;
    float total = 0.0;

    vec2 seed = gl_FragCoord.xy / mapSize; // Random seed

    for (int x = -pcfCount; x <= pcfCount; x++) {
	    for (int y = -pcfCount; y <= pcfCount; y++) {
	        // Add jittering
	        vec2 jitter = vec2(random(seed + vec2(x, y)), random(seed - vec2(x, y))) - 0.5;
	        vec2 offset = (vec2(x, y) + jitter) * texelSize;

	        float objectNearestLight = texture(shadowMap, lightCoords.xy + offset).r;
	        if (lightCoords.z > objectNearestLight + shadowBias) {
		        total += 1.0;
	        }
	    }
    }

    total /= totalTexels;

    float lightFactor = 1.0 - (total * 0.5);

    vec4 color = mix(vec4(1, 1, 0.2, 0.1), texture(gridTexture, mapCoords_FS), 0.5);

    //vec3 diff = color.xyz * diffuse(lightDirection_FS, vertexNormal_FS, intensity) * lightFactor;
    vec3 diff = 0.8 * color.xyz * lightFactor;

    float fogAlpha = 1.0 - smoothstep(1000, 2000, distance(qt_eyeVector, worldPos_FS.xyz));

    fragColor = vec4(diff, fogAlpha);

    // for testing
    //fragColor = texture(shadowMap, lightCoords.xy);

}
