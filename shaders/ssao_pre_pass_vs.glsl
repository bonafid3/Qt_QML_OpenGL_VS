#version 430

layout (location = 0) in vec3 qt_vertex;
layout (location = 1) in vec3 qt_normal;
layout (location = 2) in vec3 qt_albedo;

out vec3 worldPos_FS;
out vec3 vertexNormal_FS;
out vec3 albedo_FS;

uniform mat4 qt_modelMatrix;
uniform mat4 qt_viewMatrix;
uniform mat4 qt_projectionMatrix;
uniform mat3 qt_normalMatrix;

void main()
{
    vec4 viewPos = qt_viewMatrix * qt_modelMatrix * vec4(qt_vertex, 1.0);
    worldPos_FS = viewPos.xyz;

    vertexNormal_FS = qt_normalMatrix * qt_normal;
    
    albedo_FS = qt_albedo;

    gl_Position = qt_projectionMatrix * viewPos;
}
