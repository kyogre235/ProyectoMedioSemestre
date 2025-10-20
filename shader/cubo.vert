#version 410 core
layout (location = 0) in vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out float vWorldY;
void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    vWorldY = worldPos.y;
    gl_Position = projection * view * worldPos;
}