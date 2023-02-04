#version 330 core

// From Buffers
layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_colour;
layout (location = 2) in mat4 v_model;

out vec3 colour;

// Matrices
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

// Excess
uniform float u_Theta;

void main() {

    if (gl_InstanceID % 2 == 0) {
        colour = vec3(v_colour.b, v_colour.r, v_colour.g);
    } else {
        colour = v_colour;
    }

//    gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(v_pos, 1.0);
     gl_Position = u_ProjectionMatrix * u_ViewMatrix * v_model * vec4(v_pos, 1.0);
}