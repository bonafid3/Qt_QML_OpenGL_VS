#version 430
layout(location = 0) in vec3 qt_vertex;

out vec3 textCoords_FS;

uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_projectionMatrix;

void main() {
    textCoords_FS = qt_vertex;
    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vec4(qt_vertex, 1.0);
}
