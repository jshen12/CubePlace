#version 330 core
layout (location = 0) in vec3 aPos;   // position variable has attribute position 0
//layout (location = 1) in vec3 aColor; // color variable has attribute position 1
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aLight;

out float Light;
out vec2 TexCoord;
out float fogFactor;

uniform mat4 matrix;
uniform vec3 camera;
uniform float fog_distance;

void main()
{
   gl_Position = matrix * vec4(aPos, 1.0);  // order is important !!!! (right to left)
   //ourColor = vec3(aColor);
   Light = aLight;
   int fog_size = 16;
   float cameraDistance = distance(camera, aPos);
   fogFactor = clamp(((cameraDistance - fog_distance) / fog_size) + 1, 0.0, 1.0);
   TexCoord = aTexCoord;
}