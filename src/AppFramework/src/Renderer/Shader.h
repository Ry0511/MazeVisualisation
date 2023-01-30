//
// Created by -Ry on 26/01/2023.
//

#ifndef MAZEVISUALISATION_SHADER_H
#define MAZEVISUALISATION_SHADER_H

#include "Logging.h"
#include "Renderer/GLUtil.h"

#include <gl/glew.h>
#include <string>
#include <fstream>

namespace app {

    class Shader {

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
        explicit Shader(
                const std::string& vertex,
                const std::string& fragment
        ) {

            // Source Code
            const char* v_source = vertex.c_str();
            const char* f_source = fragment.c_str();

            // Compile Shaders
            INFO("Compiling Vertex Shader.");
            GLint v_shader = compile_shader(v_source, GL_VERTEX_SHADER);
            INFO("Compiling Fragment Shader.");
            GLint f_shader = compile_shader(f_source, GL_FRAGMENT_SHADER);

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

        ~Shader() {
            INFO("{:<20} # {}", "Destroying Shader", m_ShaderProgram);
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

    };

}

#endif //MAZEVISUALISATION_SHADER_H
