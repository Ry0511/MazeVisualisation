#version 330 core

out vec4 frag_colour;
in vec3 tex_pos;
uniform samplerCube skybox_texture;

void main() {
    frag_colour = texture(skybox_texture, tex_pos);
}
