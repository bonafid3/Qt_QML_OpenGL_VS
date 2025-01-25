#version 430

layout (binding=0) uniform sampler2D ssaoPosition;
layout (binding=1) uniform sampler2D ssaoNormal;
layout (binding=2) uniform sampler2D ssaoDepth;

uniform vec2 qt_invViewportSize;

in vec3 textCoord_FS;

out vec4 fragColor;


/*
float linearizeDepth(float depth)
{
    float zNear = 1.0;
    float zFar  = 5000.0;
    return (2.0f * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    float nonLinearDepthValue = texture(ssaoPosition, textCoord_FS.xy).r;
    float ld = linearizeDepth(nonLinearDepthValue);
    vec4 depthColor = vec4(ld, ld, ld, 1);

    //vec4 depthColor = vec4(nonLinearDepthValue, nonLinearDepthValue, nonLinearDepthValue, 1);

    fragColor = depthColor;
}
*/


#ifndef CURVATURE_OFFSET
#define CURVATURE_OFFSET 1
#endif

float curvature_soft_clamp(float curvature, float control)
{
    if (curvature < 0.5 / control)
        return curvature * (1.0 - curvature * control);
    return 0.25 / control;
}


float calculate_curvature(float ridge, float valley)
{
    vec2 uv = textCoord_FS.xy;

    vec3 offset = vec3(qt_invViewportSize, 0.0);

    float normal_up = texture(ssaoNormal, uv + offset.zy).g;
    float normal_down = texture(ssaoNormal, uv - offset.zy).g;
    float normal_right = texture(ssaoNormal, uv + offset.xz).r;
    float normal_left = texture(ssaoNormal, uv - offset.xz).r;

    float normal_diff = (normal_up - normal_down) + (normal_right - normal_left);

    if (normal_diff < 0.0)
        return -2.0 * curvature_soft_clamp(-normal_diff, valley);

    return 2.0 * curvature_soft_clamp(normal_diff, ridge);
}


void main(void)
{
    vec4 baseColor = texture2D(ssaoPosition, textCoord_FS.xy);

    float curvature = calculate_curvature(0.0, 1.0);
    baseColor.rgb *= curvature + 1.0;

    fragColor = baseColor;
}
