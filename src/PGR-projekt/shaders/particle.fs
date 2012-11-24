#version 130


uniform sampler2D tex;

in vec4 c;
in vec2 texCoord;



out vec4 fragColor;

void main()
{
    fragColor = vec4(c.r, c.g, c.b, c.a*texture(tex, texCoord).r);
}
