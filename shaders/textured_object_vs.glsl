#version 430

layout (location=0) in vec3 qt_vertex;
layout (location=1) in vec3 qt_normal;
layout (location=2) in vec3 qt_textCoord;

uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_projectionMatrix;

out vec3 textCoord_FS;

void main()
{
    textCoord_FS = qt_textCoord;
    // just rendering a quad, don't transform. must be -1 to +1
    gl_Position = vec4(qt_vertex,1);
}
