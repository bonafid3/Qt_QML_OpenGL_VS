#version 430

// note: outputting vec3 instead of vec4 for the texture coordinates
// causes that if you want to render the texture onto a quad, you will see nothing!
// don't know why, but it's like that

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 worldPos_FS;
in vec3 vertexNormal_FS;
in vec3 albedo_FS;

void main()
{
    gPosition = vec4(worldPos_FS, 1);
    gNormal = vec4(normalize(vertexNormal_FS), 1);
    gAlbedo = vec4(albedo_FS, 1);
}
