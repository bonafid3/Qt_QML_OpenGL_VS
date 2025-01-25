#version 430

layout (location=0) in vec3 qt_vertex;
layout (location=1) in vec3 qt_normal;
layout (location=2) in vec3 qt_color;

uniform mat4 qt_shadowMapMatrix;

uniform mat4 qt_modelMatrix;
uniform mat4 qt_viewMatrix;
uniform mat4 qt_projectionMatrix;
uniform mat3 qt_normalMatrix;

uniform vec3 qt_lightPositionView;

out vec3 vertexColor_FS;
out vec3 vertexNormal_FS;
out vec3 lightDirection_FS;
out vec4 shadowCoords_FS;

void main()
{
    vec4 localPos = qt_modelMatrix * vec4(qt_vertex, 1);
    vec4 worldPos = qt_viewMatrix * localPos;

    vertexColor_FS = qt_color;
    vertexNormal_FS = normalize(qt_normalMatrix * qt_normal);
    lightDirection_FS = normalize(worldPos - vec4(qt_lightPositionView,1)).xyz;

    shadowCoords_FS = qt_shadowMapMatrix * localPos;

    gl_Position = qt_projectionMatrix * worldPos;
}

