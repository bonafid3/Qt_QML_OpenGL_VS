#version 430

layout(quads, fractional_even_spacing, cw) in;

layout (binding=0) uniform sampler2D heightMap;
layout (binding=1) uniform sampler2D normalMap;

uniform mat3 qt_normalMatrix;
uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_projectionMatrix;
uniform vec3 qt_lightPositionView;

in vec2 mapCoords_TES[];
in vec4 shadowCoords_TES[];
in vec4 worldPos_TES[];

out vec2 mapCoords_FS;
out vec4 shadowCoords_FS;
out vec3 vertexNormal_FS;
out vec3 lightDirection_FS;
out vec4 worldPos_FS;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 position =
    ((1 - u) * (1 - v) * gl_in[0].gl_Position +
    u * (1 - v) * gl_in[1].gl_Position +
    u * v * gl_in[2].gl_Position +
    (1 - u) * v * gl_in[3].gl_Position);

    //worldPos_FS = worldPos_TES[0];
    worldPos_FS = position;

    vec2 mapCoords =
        ((1 - u) * (1 - v) * mapCoords_TES[0] +
        u * (1 - v) * mapCoords_TES[1] +
        u * v * mapCoords_TES[2] +
        (1 - u) * v * mapCoords_TES[3]);

    shadowCoords_FS =
        ((1 - u) * (1 - v) * shadowCoords_TES[0] +
        u * (1 - v) * shadowCoords_TES[1] +
        u * v * shadowCoords_TES[2] +
        (1 - u) * v * shadowCoords_TES[3]);

    //position.y += texture(heightMap, mapCoords).r * 32;

    vec4 normalValue = texture(normalMap, mapCoords) * 2 - 1;
    vertexNormal_FS = qt_normalMatrix * normalValue.rbg;

    lightDirection_FS = normalize(position.xyz - qt_lightPositionView);

    mapCoords_FS = mapCoords;

    gl_Position = qt_projectionMatrix * position;
}
