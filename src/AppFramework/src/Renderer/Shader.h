//
// Created by -Ry on 26/01/2023.
//

#ifndef MAZEVISUALISATION_SHADER_H
#define MAZEVISUALISATION_SHADER_H

#include "Logging.h"
#include "Renderer/GLUtil.h"

#include <string>
#include <fstream>
#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace app {

    class Shader {

    public:
        inline static const std::string s_ProjectionMatrixUniform = "u_ProjectionMatrix";
        inline static const std::string s_ViewMatrixUniform       = "u_ViewMatrix";
        inline static const std::string s_ModelMatrixUniform      = "u_ModelMatrix";
        inline static const std::string s_ThetaUniform            = "u_Theta";
        inline static const std::string s_TranslateMatrixUniform  = "u_TranslateMatrix";
        inline static const std::string s_RotateMatrixUniform     = "u_RotateMatrix";
        inline static const std::string s_ScaleMatrixUniform      = "u_ScaleMatrix";

    private:
        inline static GLuint s_EnabledShaderProgram = 0;

    private:
        GLuint m_ShaderProgram = 0;

    public:
        static std::string read_file_to_string(
                const std::string& path
        ) {
            using Iter = std::istreambuf_iterator<char>;
            std::ifstream stream(path);
            if (!stream.is_open()) {
                ERR("Failed to open file: {}", path);
                PANIC;
            }

            auto str = std::string{
                    Iter{ stream },
                    Iter{}
            };
            stream.close();

            return str;
        }

    public:
        Shader() = default;
        Shader(const Shader&) = delete;
        Shader(Shader&&) = delete;

        ~Shader() {
            HINFO("[SHDAER_DESTROY]", " # Destroying Shader '{}'", m_ShaderProgram);
            GL(glDeleteProgram(m_ShaderProgram));
        }

    private:
        __forceinline static GLuint compile_shader(
                const char* src,
                GLenum type
        ) {
            GLint handle = GL(glCreateShader(type));
            GLint state = 0;
            char  err_info[512];
            GL(glShaderSource(handle, 1, &src, nullptr));
            GL(glCompileShader(handle));
            GL(glGetShaderiv(handle, GL_COMPILE_STATUS, &state));

            if (!state) {
                GL(glGetShaderInfoLog(handle, 512, nullptr, err_info));
                ERR("Failed to compile shader. # Type: {:#08x}, Error: {}", type, err_info);
                PANIC;
            }

            return handle;
        }

    public:

        void compile_and_link(
                const std::string& vertex_shader,
                const std::string& fragment_shader
        ) {
            if (is_init()) {
                HERR(
                        "[SHADER_LINK]",
                        " # Shader Program '{}' has already been compiled and linked...",
                        m_ShaderProgram
                );
                throw std::exception();
            }

            // Source Code
            const char* v_source = vertex_shader.c_str();
            const char* f_source = fragment_shader.c_str();

            // Compile Shaders
            INFO("Compiling Vertex Shader.");
            GLint v_shader = compile_shader(read_file_to_string(v_source).c_str(), GL_VERTEX_SHADER);
            INFO("Compiling Fragment Shader.");
            GLint f_shader = compile_shader(read_file_to_string(f_source).c_str(), GL_FRAGMENT_SHADER);

            // Create the Shader Program
            m_ShaderProgram = GL(glCreateProgram());
            GL(glAttachShader(m_ShaderProgram, v_shader));
            GL(glAttachShader(m_ShaderProgram, f_shader));
            GL(glLinkProgram(m_ShaderProgram));

            // Link Programme
            GLint link_state = 0;
            GL(glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &link_state));
            if (!link_state) {
                char buffer[512]{};
                GL(glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, buffer));
                ERR("Shader Program Linker Error. # {}", buffer);
                PANIC;
            }

            // Delete these as they're linked
            GL(glDeleteShader(v_shader));
            GL(glDeleteShader(f_shader));
        }

        //############################################################################//
        // | ENABLING SHADER |
        //############################################################################//

    public:

        bool is_init() const {
            return m_ShaderProgram != 0;
        }

        void assert_enabled() const {
            ASSERT(is_enabled(), "Shader is not enabled.");
        }

        bool is_enabled() const {
            return s_EnabledShaderProgram == m_ShaderProgram;
        };

        void enable() {
            GL(glUseProgram(m_ShaderProgram));
            s_EnabledShaderProgram = m_ShaderProgram;
        }

        void disable() {
            GL(glUseProgram(0));
            s_EnabledShaderProgram = 0;
        }

        //############################################################################//
        // | ALL UNIFORMS |
        //############################################################################//

    public:

        #define SET_UNIFORM(fn) ASSERT(is_enabled(), "Shader isn't enabled..."); GL(fn)

        GLint get_uniform_location(const std::string& name) {
            GLint location = GL(glGetUniformLocation(m_ShaderProgram, name.c_str()));
            return location;
        }

        //############################################################################//
        // | FLOAT UNIFORMS |
        //############################################################################//

        void set_uniform(const std::string& name, float v) {
            SET_UNIFORM(glUniform1f(get_uniform_location(name.c_str()), v));
        }

        void set_uniform(const std::string& name, const glm::vec2& v) {
            SET_UNIFORM(glUniform2f(get_uniform_location(name.c_str()), v.x, v.y));
        }

        void set_uniform(const std::string& name, const glm::vec3& v) {
            SET_UNIFORM(glUniform3f(get_uniform_location(name.c_str()), v.x, v.y, v.z));
        }

        void set_uniform(const std::string& name, const glm::vec4& v) {
            SET_UNIFORM(glUniform4f(get_uniform_location(name.c_str()), v.x, v.y, v.z, v.w));
        }

        //############################################################################//
        // | INT UNIFORMS |
        //############################################################################//

        void set_uniform(const std::string& name, int v) {
            SET_UNIFORM(glUniform1i(get_uniform_location(name.c_str()), v));
        }

        void set_uniform(const std::string& name, const glm::ivec2& v) {
            SET_UNIFORM(glUniform2i(get_uniform_location(name.c_str()), v.x, v.y));
        }

        void set_uniform(const std::string& name, const glm::ivec3& v) {
            SET_UNIFORM(glUniform3i(get_uniform_location(name.c_str()), v.x, v.y, v.z));
        }

        void set_uniform(const std::string& name, const glm::ivec4& v) {
            SET_UNIFORM(glUniform4i(get_uniform_location(name.c_str()), v.x, v.y, v.z, v.w));
        }

        //############################################################################//
        // | MATRIX |
        //############################################################################//

        void set_uniform(const std::string& name, const glm::mat2& matrix, bool transpose = false) {
            SET_UNIFORM(glUniformMatrix2fv(
                    get_uniform_location(name),
                    1,
                    transpose ? GL_TRUE : GL_FALSE,
                    glm::value_ptr(matrix)
            ));
        }

        void set_uniform(const std::string& name, const glm::mat3& matrix, bool transpose = false) {
            SET_UNIFORM(glUniformMatrix3fv(
                    get_uniform_location(name),
                    1,
                    transpose ? GL_TRUE : GL_FALSE,
                    glm::value_ptr(matrix)
            ));
        }

        void set_uniform(const std::string& name, const glm::mat4& matrix, bool transpose = false) {
            SET_UNIFORM(glUniformMatrix4fv(
                    get_uniform_location(name),
                    1,
                    transpose ? GL_TRUE : GL_FALSE,
                    glm::value_ptr(matrix)
            ));
        }

    };

}

#endif //MAZEVISUALISATION_SHADER_H
