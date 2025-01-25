#version 330

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

// input stuff from Qt
uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_projectionMatrix;

uniform vec3 qt_cameraPos;
uniform float qt_lineWidth;

in vec3 lineColor_GS[];

out vec3 lineColor_FS;

// executes for each primitive (point)
void main()
{
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;

    vec3 u = normalize(p1 - p0);

    vec3 middleP = p0 + (u * length(p1 - p0) / 2);

    vec3 w = normalize(qt_cameraPos - middleP);

    vec3 axis = normalize(cross(w, u));

    vec4 vertexPos = vec4(p1 + axis * qt_lineWidth, 1.0); // bottom right // 1
    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
    lineColor_FS = lineColor_GS[1];
    EmitVertex();

    vertexPos = vec4(p1 - axis * qt_lineWidth, 1.0); // top right // 2
    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
    lineColor_FS = lineColor_GS[1];
    EmitVertex();

    vertexPos = vec4(p0 + axis * qt_lineWidth, 1.0); // top left   // 3
    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
    lineColor_FS = lineColor_GS[1];
    EmitVertex();

    vertexPos = vec4(p0 - axis * qt_lineWidth, 1.0); // bottom left // 4
    gl_Position = qt_projectionMatrix * qt_modelViewMatrix * vertexPos;
    lineColor_FS = lineColor_GS[1];
    EmitVertex();

    EndPrimitive();
}
