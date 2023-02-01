#version 330 core

// From Buffers
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_colour;

out vec3 colour;

// Matrices
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelMatrix;

// Excess
uniform float u_Theta;

void main() {
    colour = v_colour;
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(v_pos, 1.0);
}