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

    // Uniforms
    std::string m_ThetaUniform            = "u_Theta";
    std::string m_ProjectionMatrixUniform = "u_ProjectionMatrix";
    std::string m_ViewMatrixUniform       = "u_ViewMatrix";
    std::string m_ModelMatrixUniform      = "u_ModelMatrix";

    // Matrices
    glm::mat4 m_ProjectionMatrix = glm::mat4{ 1 };
    glm::mat4 m_ModelMatrix      = glm::mat4{ 1 };

public:
    App() : app::Application("My App", 800, 600) {}

    ~App() {
        delete m_ShaderProgram;
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        set_clear_colour({ 0.1, 0.1, 0.1, 1.0 });
        GL(glEnable(GL_DEPTH_TEST));
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(4));

        float vertex_positions[24] = {
                -1.0, 1.0, 1.0,
                -1.0, -1.0, 1.0,
                -1.0, 1.0, -1.0,
                -1.0, -1.0, -1.0,
                1.0, 1.0, 1.0,
                1.0, -1.0, 1.0,
                1.0, 1.0, -1.0,
                1.0, -1.0, -1.0
        };

        unsigned int indices[36] = {
                5 - 1, 3 - 1, 1 - 1,
                3 - 1, 8 - 1, 4 - 1,
                7 - 1, 6 - 1, 8 - 1,
                2 - 1, 8 - 1, 6 - 1,
                1 - 1, 4 - 1, 2 - 1,
                5 - 1, 2 - 1, 6 - 1,
                5 - 1, 7 - 1, 3 - 1,
                3 - 1, 7 - 1, 8 - 1,
                7 - 1, 5 - 1, 6 - 1,
                2 - 1, 4 - 1, 8 - 1,
                1 - 1, 3 - 1, 4 - 1,
                5 - 1, 1 - 1, 2 - 1
        };

        m_Ivo.init();
        m_Ivo.bind();

        app::FloatAttribPtr attrib{ 0, 3 };
        m_Ivo.add_vertex_buffer(vertex_positions, 24, attrib);
        m_Ivo.set_index_buffer(indices, 36);

        float vertex_colours[]{
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0,
                0.0, 1.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 0.0, 1.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                1.0, 0.0, 1.0,
                0.0, 1.0, 0.0,
        };

        app::FloatAttribPtr color_attrib{ 1, 3 };
        m_Ivo.add_vertex_buffer(vertex_colours, 36, color_attrib);

        m_ShaderProgram = new app::Shader(
                app::Shader::read_file_to_string("Res/Shaders/VertexShader.glsl"),
                app::Shader::read_file_to_string("Res/Shaders/FragmentShader.glsl")
        );
        m_ShaderProgram->enable();

        // Initial Projection & View Matrix
        m_ProjectionMatrix = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.1f, 1000.f);
    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;
        set_title(std::format(
                "Window # {:6f}, {:4f}, {:4f}",
                delta,
                m_Theta,
                abs(sin(m_Theta * 2))).c_str()
        );
        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);
        clear();

        // Rotate the Model to see all sides
        float x_angle = sin(m_Theta);
        float y_angle = sin(-(m_Theta * 1.35));
        float z_angle = sin(m_Theta * 1.25);

        m_ModelMatrix = glm::rotate(glm::mat4{1}, m_Theta, {0.2, 0.15, 0.8});

        // Push Uniforms
        if (m_ShaderProgram == nullptr) throw std::exception();
        m_ShaderProgram->enable();
        m_ShaderProgram->set_uniform(m_ThetaUniform, m_Theta);
        m_ShaderProgram->set_uniform(m_ProjectionMatrixUniform, m_ProjectionMatrix);
        m_ShaderProgram->set_uniform(m_ViewMatrixUniform, get_camera_matrix());
        m_ShaderProgram->set_uniform(m_ModelMatrixUniform, m_ModelMatrix);

        m_Ivo.bind();

        draw_elements(app::DrawMode::TRIANGLES, 36);

        int buffer[18] {
           1, 0, 0,
           0, 1, 0,
           0, 0, 1,

           1, 1, 0,
           1, 0, 1,
           0, 1, 1
        };

        for (int i = 0; i < 18; ++i) {
            float x = buffer[i], y = buffer[++i], z = buffer[++i];
            float dx = 0, dy = 0, dz = 0;

            if (x < 0.9F) dx = 8;
            if (y < 0.9F) dy = 8;
            if (z < 0.9F) dz = 8;

            m_ModelMatrix = glm::rotate(glm::mat4{1}, m_Theta, {x, y, z})
                    * glm::translate(glm::mat4{1}, {dx, dy, dz})
                    * glm::rotate(glm::mat4{ 1 }, m_Theta, { x, y, z });
            m_ShaderProgram->set_uniform(m_ModelMatrixUniform, m_ModelMatrix);
            draw_elements(app::DrawMode::TRIANGLES, 36);

            m_ModelMatrix = glm::rotate(glm::mat4{1}, m_Theta, {x, y, z})
                            * glm::translate(glm::mat4{1}, {-dx, -dy, -dz})
                            * glm::rotate(glm::mat4{ 1 }, m_Theta, { x, y, z });
            m_ShaderProgram->set_uniform(m_ModelMatrixUniform, m_ModelMatrix);
            draw_elements(app::DrawMode::TRIANGLES, 36);
        }

        m_Ivo.unbind();

        m_ShaderProgram->disable();

        return true;
    }
};

int main() {
    auto app = App();
    app.start();
}