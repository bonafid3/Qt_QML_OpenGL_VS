#version 430

layout (location=0) in vec3 qt_vertex;
layout (location=1) in vec3 qt_normal;
layout (location=2) in vec3 qt_textCoord;

uniform mat4 qt_offsetMatrix;
uniform float qt_aspectRatio;

out vec2 textCoord_FS;

void main()
{
    // offsetMatrix is identity, so this won't change qt_vertex
    vec4 transformedPos = qt_offsetMatrix * vec4(qt_vertex, 1);
    
    textCoord_FS = qt_textCoord.xy;
    // just rendering a quad, don't transform. must be -1 to +1
    gl_Position = transformedPos;
}
