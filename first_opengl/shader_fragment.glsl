#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

in float fogFactor;

uniform sampler2D ourTexture1;

void main()
{
    vec4 texColor = texture(ourTexture1, TexCoord);// * vec4(ourColor, 1.0);
    if (texColor.a < 0.1)   // transparency check
        discard;
    

    vec4 fog_color = vec4(120.0 / 256.0, 167.0 / 256.0, 255.0 / 256.0, 1.0);
    texColor = mix(texColor, fog_color, fogFactor);
    FragColor = texColor;
}