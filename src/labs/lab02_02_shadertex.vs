#version 130 

in vec3 position, normal;

uniform mat4 mvp, mv;
uniform mat3 mn;

//light
uniform vec3 lightPosition;

//material
uniform float shininess;

out vec2 light;//ambient+diffuse, specular

out vec3 modelPosition, eyePosition, eyeNormal;

void main()
{
    gl_Position = mvp*vec4(position,1);

    modelPosition = position;
    eyePosition = (mv*vec4(position,1)).xyz;
    eyeNormal = normalize(mn*normal);
    
    vec3 N = eyeNormal;
    vec3 L = normalize(lightPosition-eyePosition);

    //Blin-Phong model
    light = vec2(0.1, 0.0);
    float diffuse = dot(L, N);
    if(diffuse >= 0.0)
    {
        light.x += diffuse;

        vec3 V = normalize(-eyePosition);
        vec3 H = normalize(L + V);
        float specular = pow(dot(N, H), shininess);

        if(specular >= 0.0)
        {
            light.y += specular;
        }
    }
}
