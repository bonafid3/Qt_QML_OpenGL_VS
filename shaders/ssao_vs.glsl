#version 430

layout (location=0) in vec3 qt_vertex;
layout (location=1) in vec3 qt_normal;
layout (location=2) in vec3 qt_textCoord;

uniform float qt_aspectRatio;

out vec2 textCoord_FS;

void main()
{
    //textCoord_FS = qt_textCoord.xy * vec2(1.0, qt_aspectRatio);
    textCoord_FS = qt_textCoord.xy;
    // just rendering a quad, don't transform. must be -1 to +1
    gl_Position = vec4(qt_vertex, 1.0);
}
