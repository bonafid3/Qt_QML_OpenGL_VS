#version 430

layout (location=0) in vec3 position;

uniform mat4 qt_shadowMapMatrix;
uniform mat4 qt_modelMatrix;
uniform mat4 qt_viewMatrix;

out vec2 mapCoords_TCS;
out vec4 shadowCoords_TCS;

out vec4 worldPos_TCS;

void main()
{
    worldPos_TCS = qt_modelMatrix * vec4(position, 1); // maybe good idea to pass for fog calculation

    mapCoords_TCS = position.xz / 50;
    shadowCoords_TCS = qt_shadowMapMatrix * worldPos_TCS;

    gl_Position = qt_viewMatrix * worldPos_TCS;
}
