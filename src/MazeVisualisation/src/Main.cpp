//
// Created by -Ry on 16/01/2023.
//

#include "Application.h"

#include "RenderableCube.h"

#include "Logging.h"
#include "Renderer/GLUtil.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexObjectBinding.h"

class App : public app::Application {

private:
    float m_Theta = 0.0F;

    app::Shader* m_ShaderProgram = nullptr;

    // Uniforms
    std::string                       m_ThetaUniform            = "u_Theta";
    std::string                       m_ProjectionMatrixUniform = "u_ProjectionMatrix";
    std::string                       m_ViewMatrixUniform       = "u_ViewMatrix";
    std::string                       m_ModelMatrixUniform      = "u_ModelMatrix";
    std::vector<maze::RenderableCube> m_Cubes{};
    app::redone::Vao                  m_Vao{};
    size_t                            m_GridSize                = 256;

    // Matrices
    glm::mat4 m_ProjectionMatrix = glm::mat4{ 1 };

public:
    App() : app::Application("My App", 800, 600) {}

    ~App() {
        delete m_ShaderProgram;
    }

    virtual void camera_update(app::Window& window, float delta) override {
        Camera3D::camera_update(window, delta);
        get_camera_state().cam_pos.y = 3.F;
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        set_clear_colour({ 0.1, 0.1, 0.1, 1.0 });
        GL(glEnable(GL_DEPTH_TEST));
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(4));

        m_ShaderProgram = new app::Shader(
                app::Shader::read_file_to_string("Res/Shaders/VertexShaderInstanced.glsl"),
                app::Shader::read_file_to_string("Res/Shaders/FragmentShader.glsl")
        );
        m_ShaderProgram->enable();

        // Initial Projection & View Matrix
        m_ProjectionMatrix = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.1f, 1000.f);

        int grid_size = m_GridSize;

        for (int i = 0; i < grid_size; ++i) {
            for (int j = 0; j < grid_size; ++j) {
                maze::RenderableCube cube{};
                cube.get_transform().translate = { i, 0, j };
                cube.get_transform().scale     = { 0.25, 0.25, 0.25 };
                m_Cubes.emplace_back(cube);
            }
        }

        m_Vao.init();
        m_Vao.bind();

        // Vertex Buffer
        app::redone::SimpleBuffer vertex_buffer = app::redone::array_buffer();
        vertex_buffer.init();
        vertex_buffer.bind();
        vertex_buffer.set_data_static<float>(maze::cube_obj::s_VertexPositions.data(), 24);
        m_Vao.add_buffer(vertex_buffer);
        auto vertex_attrib = std::make_unique<app::redone::Vec3Attribute>(0);
        vertex_attrib->create_and_enable();
        m_Vao.add_attribute(std::move(vertex_attrib));

        // Index Buffer
        m_Vao.set_index_buffer(maze::cube_obj::s_Indices.data(), 36);

        // Colour Buffer
        app::redone::SimpleBuffer colour_buffer = app::redone::array_buffer();
        colour_buffer.init();
        colour_buffer.bind();
        colour_buffer.set_data_static<float>(maze::cube_obj::s_Colours.data(), 24);
        m_Vao.add_buffer(colour_buffer);
        auto colour_attrib = std::make_unique<app::redone::Vec3Attribute>(1);
        colour_attrib->create_and_enable();
        m_Vao.add_attribute(std::move(colour_attrib));

        // Matrix Buffer
        std::vector<glm::mat4> translates{};
        for (auto& cube : m_Cubes) {
            cube.update(0.008);
            translates.emplace_back(cube.get_model_matrix());
        }

        app::redone::SimpleBuffer matrix_buffer = app::redone::array_buffer();
        matrix_buffer.init();
        matrix_buffer.bind();
        matrix_buffer.set_data_dynamic<glm::mat4>(translates.data(), translates.size());
        auto matrix_attrib = std::make_unique<app::redone::FloatMat4Attrib>(2);
        matrix_attrib->create_and_enable();
        m_Vao.add_buffer(matrix_buffer);
        m_Vao.add_attribute(std::move(matrix_attrib));

        m_Vao.unbind();

    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;
        set_title(std::format(
                "Window # {}fps, Delta: {:6f}, Theta: {:4f}, {:4f}",
                (int) (1.0 / (delta)),
                delta,
                m_Theta,
                abs(sin(m_Theta * 2))).c_str()
        );
        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);
        clear();

        // Push Uniforms
        if (m_ShaderProgram == nullptr) throw std::exception();
        m_ShaderProgram->enable();
        m_ShaderProgram->set_uniform(m_ThetaUniform, m_Theta);
        m_ShaderProgram->set_uniform(m_ProjectionMatrixUniform, m_ProjectionMatrix);
        m_ShaderProgram->set_uniform(m_ViewMatrixUniform, get_camera_matrix());

        m_Vao.bind_all();

        std::vector<glm::mat4> translates{};
        for (auto& cube : m_Cubes) {
            cube.update(delta);
            translates.emplace_back(cube.get_model_matrix());
        }
        m_Vao.get_buffer(2).set_data_dynamic<glm::mat4>(translates.data(), translates.size());


        draw_elements_instanced(app::DrawMode::TRIANGLES, 36, translates.size());
        m_Vao.unbind();

        m_ShaderProgram->disable();

        return true;
    }
};

int main() {
    auto app = App();
    app.start();
}