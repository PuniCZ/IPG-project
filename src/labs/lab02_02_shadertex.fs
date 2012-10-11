#version 130 

//light
uniform vec3 lightPosition;

//material
uniform float shininess;

uniform float mode;

in vec2 light; 

in vec3 modelPosition, eyePosition, eyeNormal;

out vec4 fragColor;

void main()
{
    vec3 N = normalize(eyeNormal);
    vec3 L = normalize(lightPosition-eyePosition);

    //Texture
    vec3 color = mix(vec3(1, 0, 0), vec3(0, 1, 0), fract(length(modelPosition)));

    //Blin-Phong model
    vec3 lightColor = 0.1*color;
    float diffuse = dot(L, N);
    if(diffuse >= 0.0)
    {
        lightColor += diffuse*color;

        vec3 V = normalize(-eyePosition);
        vec3 H = normalize(L + V);
        float specular = pow(dot(N, H), shininess);

        if(specular >= 0.0f)
        {
            lightColor += vec3(specular);
        }
    }

    if(mode < 0.5)
        fragColor = vec4(light.x*color+vec3(light.y), 1.0);
    else
        fragColor = vec4(lightColor, 1.0);
}
