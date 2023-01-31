#version 330 core

layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec3 v_colour;

in float u_Theta;

out vec3 colour;

void main() {
    float sin_offset = abs(sin(u_Theta * 2));

    if (sin_offset <= 0.2) {
        colour = vec3(1, 0, 1);
    } else {
        colour = v_colour * sin_offset;
    }
    gl_Position = vec4(v_pos, 1.0, 1.0);
}