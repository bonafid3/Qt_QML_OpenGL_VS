#version 430

layout (binding=0) uniform samplerCube skybox;

in vec3 textCoords_FS;
out vec4 fragColor;

void main() {
    fragColor = texture(skybox, textCoords_FS);
}
