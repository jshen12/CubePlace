#version 330 core
layout (location = 0) in vec3 aPos;   // position variable has attribute position 0
//layout (location = 1) in vec3 aColor; // color variable has attribute position 1
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;                    // specify color output to fragment shader
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 offset;
uniform int TexSize;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);  // order is important !!!! (right to left)
   //ourColor = vec3(aColor);
   TexCoord = vec2(aTexCoord.x/TexSize, aTexCoord.y/TexSize) + offset;
}