#version 330

in vec3 lineColor_FS;

out vec4 fragColor;

void main()
{
    fragColor = vec4(lineColor_FS, 1.0);
}
