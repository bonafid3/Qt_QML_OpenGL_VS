#version 430

layout (binding=0) uniform sampler2D ssaoInput;

in vec2 textCoord_FS;

out vec4 fragColor;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, textCoord_FS + offset).r;
        }
    }
    result = result / (4.0 * 4.0);
    fragColor = vec4(vec3(result), 1.0);
}
