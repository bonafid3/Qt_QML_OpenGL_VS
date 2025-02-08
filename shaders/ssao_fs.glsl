#version 430

layout (binding=0) uniform sampler2D gPosition;
layout (binding=1) uniform sampler2D gNormal;
layout (binding=2) uniform sampler2D texNoise;

in vec2 textCoord_FS;

uniform vec3 samples[64];

out vec4 fragColor;

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 3.0;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 qt_noiseScale;   // screenWidth/4, screenHeight/4

uniform mat4 qt_projectionMatrix;

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, textCoord_FS).xyz;
    vec3 normal = normalize(texture(gNormal, textCoord_FS).rgb);
    vec3 randomVec = normalize(texture(texNoise, textCoord_FS * qt_noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = qt_projectionMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }

    float aoStrength = 2; // user-controllable parameter
    occlusion = 1.0 - (occlusion / kernelSize) * aoStrength;

    fragColor = vec4(vec3(occlusion), 1);
    
    vec4 screenPos = vec4(fragPos, 1.0);
    screenPos = qt_projectionMatrix * screenPos; // from view to clip-space
    screenPos.xyz /= screenPos.w; // perspective divide
    screenPos.xyz = screenPos.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
    if (screenPos.z < 1.0) { // not on background
        fragColor = vec4(vec3(occlusion), 1);
    } else {
        fragColor = vec4(0.8); // background color
    }
}
