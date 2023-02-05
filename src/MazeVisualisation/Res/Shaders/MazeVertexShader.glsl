#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 m_colour;
layout (location = 2) in vec3 m_pos;

out vec3 colour;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_RotateMatrix;
uniform mat4 u_ScaleMatrix;
uniform float u_Theta;

void main() {
    colour = m_colour;

    mat4 translate = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(m_pos, 1)
    );
    mat4 model = translate * u_RotateMatrix * u_ScaleMatrix;

    gl_Position = u_ProjectionMatrix * u_ViewMatrix * model * vec4(v_pos, 1.0);
}
