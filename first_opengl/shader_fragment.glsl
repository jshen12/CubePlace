#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

in float fogFactor;

uniform sampler2D ourTexture1;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    //ambient 
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);  // change for directional light
    //vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0));
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    vec4 texColor = texture(ourTexture1, TexCoord) * vec4(diffuse + ambient, 1.0f);
    if (texColor.a < 0.1)   // transparency check
        discard;

    vec4 fog_color = vec4(120.0 / 256.0, 167.0 / 256.0, 255.0 / 256.0, 1.0);
    texColor = mix(texColor, fog_color, fogFactor);
    FragColor = texColor;
}