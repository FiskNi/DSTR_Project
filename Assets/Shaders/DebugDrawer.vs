#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 f_color;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
    gl_Position = projMatrix * viewMatrix * vec4(position, 1.0f);

   f_color = color;
}