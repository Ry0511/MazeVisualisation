#version 330 core

layout (location = 0) in vec3 v_vert_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_tex;
layout (location = 3) in mat4 v_model_matrix;
//layout (location = 5) in vec3 v_colour;

out vec3 colour;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_RotateMatrix;
uniform mat4 u_ScaleMatrix;

void main() {
    colour = vec3(0,0.75,1.0);
    // colour = v_colour;
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * v_model_matrix * vec4(v_vert_pos, 1.0);
}
