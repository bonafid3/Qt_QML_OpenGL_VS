#version 430

layout (binding=0) uniform sampler2D shadowMap;

in vec4 shadowCoords_FS;

in vec3 vertexColor_FS;
in vec3 vertexNormal_FS;
in vec3 lightDirection_FS;

out vec4 fragColor;

float diffuse(vec3 direction, vec3 normal, float intensity)
{
    return max(0.0, dot(-direction, normal)) * intensity;
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
    vec3 lightCoords = shadowCoords_FS.xyz / shadowCoords_FS.w;

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

    float lightFactor = 1.0 - (total * 0.66);

    vec3 diff = vertexColor_FS * diffuse(lightDirection_FS, vertexNormal_FS, intensity) * lightFactor;
    fragColor = vec4(diff, 1.0);
}
