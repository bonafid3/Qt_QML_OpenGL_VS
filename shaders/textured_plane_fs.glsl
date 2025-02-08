#version 430

layout (binding=0) uniform sampler2D superTexture;

in vec2 textCoord_FS;

out vec4 fragColor;

//float linearizeDepth(float depth)
//{
//    float zNear = 0.5;
//    float zFar  = 5000.0;
//    return (2.0f * zNear) / (zFar + zNear - depth * (zFar - zNear));
//}

void main()
{
    //float nonLinearDepthValue = texture(superTexture, textCoord_FS).r;
    //float ld = linearizeDepth(nonLinearDepthValue);
    //vec4 depthColor = vec4(ld, ld, ld, 1.0);
    //vec4 depthColor = vec4(nonLinearDepthValue, nonLinearDepthValue, nonLinearDepthValue, 1.0);
    //fragColor = depthColor;

    vec3 mappedPos = texture(superTexture, textCoord_FS).rgb;
    fragColor = vec4(mappedPos, 1.0);
    //fragColor = texture(superTexture, textCoord_FS);
}
