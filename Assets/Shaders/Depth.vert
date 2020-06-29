#version 430

layout(location = 0) in vec3 position;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1);

}
