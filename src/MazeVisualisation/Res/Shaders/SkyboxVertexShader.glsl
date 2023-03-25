#version 330 core

layout (location = 0) in vec3 v_vert_pos;

out vec3 tex_pos;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main() {
    tex_pos = v_vert_pos;
    vec4 pos = u_ProjectionMatrix * u_ViewMatrix * vec4(v_vert_pos, 1.0);
    gl_Position = pos.xyww;
}
