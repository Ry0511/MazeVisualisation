//
// Created by -Ry on 16/01/2023.
//

#include "Application.h"

#include "Logging.h"
#include "Renderer/VertexArrayObject.h"
#include "Renderer/GLUtil.h"
#include "Renderer/Shader.h"

class App : public app::Application {

private:
    float m_Theta = 0.0F;

    app::Shader* m_ShaderProgram = nullptr;
    app::IndexedVertexObject m_Ivo{};
    std::string m_ThetaUniform = "u_Theta";

public:
    App() : app::Application("My App", 800, 600) {}

    ~App() {
        delete m_ShaderProgram;
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        GL(glLoadIdentity());
        GL(glOrtho(-1, 1, -1, 1, 0, 1));
//        GL(glClearColor(0.1, 0.1, 0.1, 1.0));
        set_clear_colour({ 0.1, 0.1, 0.1, 1.0 });
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(4));

        float vertex_positions[] = {
                -0.9, 0.9,
                0.9, 0.0,
                -0.9, -0.9
        };

        unsigned int indices[] = {
                0, 1, 2
        };

        m_Ivo.init();
        m_Ivo.bind();

        app::FloatAttribPtr attrib{ 0, 2 };
        m_Ivo.add_vertex_buffer(vertex_positions, 6, attrib);
        m_Ivo.set_index_buffer(indices, 3);

        float               vertex_colours[]{
                1.0, 1.0, 1.0,
                0.5, 0.0, 0.5,
                0.2, 1.0, 0.2
        };
        app::FloatAttribPtr color_attrib{ 1, 3 };
        m_Ivo.add_vertex_buffer(vertex_colours, 9, color_attrib);

        m_ShaderProgram = new app::Shader(
                app::Shader::read_file_to_string("Res/Shaders/VertexShader.glsl"),
                app::Shader::read_file_to_string("Res/Shaders/FragmentShader.glsl")
        );
        m_ShaderProgram->enable();

    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;
        set_title(std::format("Window # {:6f}, {:4f}, {:4f}", delta, m_Theta, abs(sin(m_Theta * 2))).c_str());
        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);
        clear();

        m_ShaderProgram->enable();
        m_ShaderProgram->set_uniform(m_ThetaUniform, m_Theta);

        m_Ivo.bind();
        draw_elements(app::DrawMode::TRIANGLES, 3);
        m_Ivo.unbind();

        m_ShaderProgram->disable();

        return true;
    }
};

int main() {
    auto app = App();
    app.start();
}