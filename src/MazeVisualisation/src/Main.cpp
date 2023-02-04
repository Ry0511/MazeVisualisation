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
    std::string m_ThetaUniform            = "u_Theta";
    std::string m_ProjectionMatrixUniform = "u_ProjectionMatrix";
    std::string m_ViewMatrixUniform       = "u_ViewMatrix";
    app::Vao    m_Vao{};
    size_t      m_GridSize                = 128;

    size_t m_TickCount         = 0;
    float  m_FrameTimeTotal    = 0.0f;
    size_t m_MaxFrameTimeCount = 5000;

    // Matrices
    glm::mat4 m_ProjectionMatrix = glm::mat4{ 1 };
    glm::mat4 m_ModelMatrix      = glm::mat4{ 1 };

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
        GL(glEnable(GL_STENCIL_TEST));
        GL(glEnable(GL_DEPTH_TEST));
        GL(glEnable(GL_CULL_FACE));
        GL(glFrontFace(GL_CCW));
        GL(glCullFace(GL_BACK));
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(4));

        m_ShaderProgram = new app::Shader(
                app::Shader::read_file_to_string("Res/Shaders/MazeVertexShader.glsl"),
                app::Shader::read_file_to_string("Res/Shaders/FragmentShader.glsl")
        );
        m_ShaderProgram->enable();

        // Initial Projection & View Matrix
        m_ProjectionMatrix = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.1f, 100.f);

        m_Vao.init();
        m_Vao.bind();

        // Vertex Buffer
        app::SimpleBuffer vertex_buffer = app::array_buffer();
        vertex_buffer.init();
        vertex_buffer.bind();
        vertex_buffer.set_data_static<float>(maze::cube_obj::s_VertexPositions.data(), 24);
        m_Vao.add_buffer<app::Vec3Attribute>(vertex_buffer, 0U);

        // Index Buffer
        m_Vao.set_index_buffer(maze::cube_obj::s_Indices.data(), 36);

        // Colour Buffer
        app::SimpleBuffer colour_buffer = app::array_buffer();
        colour_buffer.init();
        colour_buffer.bind();
        colour_buffer.set_data_static<float>(maze::cube_obj::s_Colours.data(), 24);
        m_Vao.add_buffer<app::Vec3Attribute>(colour_buffer, 1U);

        // Vertex Position Buffer
        app::SimpleBuffer position_buffer = app::array_buffer();
        position_buffer.init();
        position_buffer.bind();
        position_buffer.set_data_static<glm::vec3>(nullptr, m_GridSize * m_GridSize);
        m_Vao.add_buffer<app::Vec3Attribute>(position_buffer, 2U, 1);

        // Generate Cubes
        maze::CubeManager& manager = add_component<maze::CubeManager>(create_entity(), m_Vao);

        for (size_t i = 0; i < m_GridSize; ++i) {
            for (size_t j = 0; j < m_GridSize; ++j) {
                app::Entity entity = create_entity();
                add_component<maze::Cube>(entity, entity);
                add_component<app::Position>(entity, glm::vec3{ i, 0, j });
            }
        }
        manager.init(this);

        m_Vao.unbind();

    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;

        ++m_TickCount;
        m_FrameTimeTotal += delta;
        float avg = m_FrameTimeTotal / m_TickCount;
        if (m_TickCount >= m_MaxFrameTimeCount) m_TickCount = 0;

        set_title(
                std::format(
                        "Window # {}fps, Delta: {:6f}, Theta: {:4f}, Avg: {:4f}",
                        (int) (1.0 / (delta)),
                        delta,
                        m_Theta,
                        avg
                ).c_str()
        );
        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);
        Renderer::clear();

        // Push Uniforms
        if (m_ShaderProgram == nullptr) throw std::exception();
        m_ShaderProgram->enable();

        // Uniforms
        m_ShaderProgram->set_uniform(m_ThetaUniform, m_Theta);
        m_ShaderProgram->set_uniform(m_ProjectionMatrixUniform, m_ProjectionMatrix);
        m_ShaderProgram->set_uniform(m_ViewMatrixUniform, get_camera_matrix());

        auto             view = get_view<maze::CubeManager>();
        for (app::Entity id : view) {
            maze::CubeManager& manager = view.get<maze::CubeManager>(id);
            manager.update(delta, this);
            manager.render(this, m_ShaderProgram);
        }

        m_ShaderProgram->disable();

        return true;
    }
};

int main() {
    auto app = new App();
    app->start();
    delete app;
}