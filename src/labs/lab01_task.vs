#version 130

in vec3 position;
in vec4 color;

uniform float aspect;//fow 45, near 1 and far 1000 are hardcoded
uniform float translate;
uniform vec2 rotate;
uniform float scale;

out vec4 c;

void main()
{
    vec4 pos = vec4(position,1);
    //scale
    pos.xyz *= scale;
    //rotate around y
    pos = vec4(
        cos(rotate.y)*pos.x + sin(rotate.y)*pos.z,
        pos.y,
        -sin(rotate.y)*pos.x + cos(rotate.y)*pos.z,
        1);
    //rotate around x
    pos = vec4(
        pos.x,
        cos(rotate.x)*pos.y - sin(rotate.x)*pos.z,
        sin(rotate.x)*pos.y + cos(rotate.x)*pos.z,
        1);
    //translate
    pos.z += translate;
    //perspective project with fow 45, near 1 and far 1000
    float f = 1.0/tan(45.0/2.0);
    pos = vec4(
        pos.x*f/aspect,
        pos.y*f,
        pos.z*(1.0+1000.0)/(1.0-1000.0) + pos.w*2.0*1.0*1000.0/(1.0-1000.0),
        -pos.z);
    //VS outputs
    gl_Position = pos;
    c = color;
}
