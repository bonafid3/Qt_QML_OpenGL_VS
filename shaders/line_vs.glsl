#version 330

layout (location=0) in vec3 qt_vertex;
layout (location=1) in vec3 qt_color;

uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_projectionMatrix;

out vec3 lineColor_GS;

void main()
{
    lineColor_GS = qt_color;
    gl_Position = vec4(qt_vertex, 1.0);
}
