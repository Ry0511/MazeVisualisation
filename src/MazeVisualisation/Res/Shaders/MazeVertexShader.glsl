#version 330 core

layout (location = 0) in vec3 v_vert_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_tex;
layout (location = 3) in vec3 v_colour;
layout (location = 4) in mat4 v_model_matrix;

out vec3 colour;
out vec3 normal;
out vec3 frag_pos;
out vec2 tex_pos;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ScaleMatrix;

void main() {
    colour = v_colour;
    normal = v_normal;
    tex_pos = vec2(v_tex);

    mat4 model = u_ScaleMatrix * v_model_matrix;
    frag_pos = vec3(model * vec4(v_vert_pos, 1.0));
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * model * vec4(v_vert_pos, 1.0);
}
