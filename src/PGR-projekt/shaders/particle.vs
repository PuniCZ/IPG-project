#version 130

in vec3 position;
in vec4 color;
in vec2 texCoordIn;

out vec2 texCoord;



uniform mat4 mvp;


out vec4 c;

void main()
{

    //VS outputs
    gl_Position = mvp * vec4(position,1);
    c = color;
    texCoord = texCoordIn;
}
