#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aAO;

out vec2 TexCoord;
out vec3 posSCO;
out float ao;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 pos = view * model * vec4(aPos, 1.0);
    posSCO = pos.xyz;
    ao = aAO;
    gl_Position = projection * pos;
    TexCoord = aTexCoord;
}