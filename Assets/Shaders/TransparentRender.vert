#version 430


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

//uniform float firespeed;
//uniform float pulseHeight;
//uniform float displacementHeight;
//uniform float turbulenceDetail;

out vec2 f_UV;
out vec3 f_normal;
out vec4 f_position;


void main() {

    f_position = modelMatrix * vec4(position, 1.0f);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
    f_UV.x = uv.x;
    f_UV.y = -uv.y;
    f_normal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);
}
