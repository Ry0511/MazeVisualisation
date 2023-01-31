#version 330 core

layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec3 v_colour;

uniform float u_Theta;

out vec3 colour;

void main() {
    float sin_offset = abs(sin(u_Theta * 2));

    colour = v_colour * sin_offset;

    gl_Position = vec4(v_pos, 1.0, 1.0);
}