#version 430


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 f_UV;
out vec3 f_normal;
out vec4 f_position;
out vec3 f_tangent;

void main() 
{
    f_position = modelMatrix * vec4(position, 1.0f);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
    f_UV.x = uv.x;
    f_UV.y = -uv.y;
    f_normal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);
	f_tangent = vec3(modelMatrix * vec4(tangent, 0.0f));
}
