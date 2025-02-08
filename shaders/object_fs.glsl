#version 430

layout (binding=0) uniform sampler2D shadowMap;
layout (binding=1) uniform sampler2D ssaoMap;

in vec4 shadowCoords_FS;

in vec3 vertexColor_FS;
in vec3 fragPosView_FS;
in vec3 vertexNormal_FS;
in vec3 lightDirection_FS;

uniform vec3 qt_eyePositionView;

out vec4 fragColor;

float random(vec2 seed)
{
    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

const float intensity = 1.0;
const int pcfCount = 2;
const float totalTexels = (pcfCount * 2.0 + 1.0) * (pcfCount * 2.0 + 1.0);

void main()
{
    const float shadowBias = 0.00001; // don't use, it shifts the shadow (Peter Panning)
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

    float lightFactor = 1.0 - (total * 0.5); // shadow factor

    // Ambient component
    ivec2 ssaoMapSize = textureSize(ssaoMap, 0); // Get the actual size of the ssaoMap texture
    vec2 textCoord = gl_FragCoord.xy / vec2(ssaoMapSize); // Normalize using the correct texture dimensions
    vec4 ambientOcclusion = texture(ssaoMap, textCoord); // Sample the ssaoMap using correct texture coordinates
    vec3 ambient = (0.5 * vertexColor_FS * ambientOcclusion.r);

    // Diffuse component
    float diff = max(dot(normalize(vertexNormal_FS), lightDirection_FS), 0.0);
    vec3 diffuse = diff * vertexColor_FS;

    fragColor = vec4((ambient + diffuse) * lightFactor, 1.0);
}
