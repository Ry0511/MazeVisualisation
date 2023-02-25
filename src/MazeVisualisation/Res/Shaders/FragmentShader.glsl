#version 330 core

// Input From Vertex Shader
in vec3 colour;
in vec3 normal; // Note that this is not normalised and can be wrong if non-uniform scaling is applied.
in vec3 frag_pos;

// Fragment Output
out vec4 frag_colour;

// Lighting Params
uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_LightColour;
uniform float u_Ambient;
uniform float u_Specular;
uniform float u_Shininess;

// Calculates Ambient Lighting
vec3 get_ambient_lighting() {
    return (u_Ambient * u_LightColour) * colour;
}

// Calculates Diffuse Lighting
vec3 get_diffuse_lighting() {
    vec3 norm = normalize(normal);
    vec3 dir = normalize(u_LightPos - frag_pos);
    float diff = max(dot(norm, dir), 0.0);
    return diff * u_LightColour;
}

// Calculates Light Colour
vec3 get_specular_lighting() {
    vec3 dir = normalize(u_LightPos - frag_pos);
    vec3 reflect_dir = reflect(-u_LightDir, normalize(normal));
    float specular = pow(max(dot(dir, reflect_dir), 0.0), u_Shininess);
    return u_Specular * specular * u_LightColour;
}

void main() {
    vec3 ambient = get_ambient_lighting();
    vec3 diffuse = get_diffuse_lighting();
    vec3 specular = get_specular_lighting();
    frag_colour = vec4((ambient + diffuse + specular) * colour, 1.0);
}
