#version 430

// this shader is used only to render into a depth buffer, so the fragment shader is empty!

layout (location=0) in vec3 qt_vertex;
uniform mat4 qt_modelViewProjectionMatrix;

void main()
{
    gl_Position = qt_modelViewProjectionMatrix * vec4(qt_vertex, 1);
}
