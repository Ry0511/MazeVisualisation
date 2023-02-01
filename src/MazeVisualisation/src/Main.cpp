//
// Created by -Ry on 16/01/2023.
//

#include "Application.h"

#include "RenderableCube.h"

#include "Logging.h"
#include "Renderer/GLUtil.h"
#include "Renderer/Shader.h"

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

    // Matrices
    glm::mat4 m_ProjectionMatrix = glm::mat4{ 1 };

public:
    App() : app::Application("My App", 800, 600) {}

    ~App() {
        delete m_ShaderProgram;
    }

    virtual void camera_update(app::Window& window, float delta) override {
        Camera3D::camera_update(window, delta);
        get_camera_state().cam_pos.y = 0.F;
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        set_clear_colour({ 0.1, 0.1, 0.1, 1.0 });
        GL(glEnable(GL_DEPTH_TEST));
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(4));


        m_ShaderProgram = new app::Shader(
                app::Shader::read_file_to_string("Res/Shaders/VertexShader.glsl"),
                app::Shader::read_file_to_string("Res/Shaders/FragmentShader.glsl")
        );
        m_ShaderProgram->enable();

        // Initial Projection & View Matrix
        m_ProjectionMatrix = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.1f, 1000.f);

        int grid_size = 128;

        for (int i = 0; i < grid_size; ++i) {
            for (int j = 0; j < grid_size; ++j) {
                maze::RenderableCube cube{};
                cube.get_transform().translate = { i, 0, j};
                cube.get_transform().scale = {0.25, 0.25, 0.25};
                m_Cubes.emplace_back(std::move(cube));
            }
        }
    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;
        set_title(std::format(
                "Window # Delta: {:6f}, Theta: {:4f}, {:4f}",
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

        for (auto& cube : m_Cubes) {
            cube.update(delta);
            cube.render_singular(*this, *m_ShaderProgram);
        }

        m_ShaderProgram->disable();

        return true;
    }
};

int main() {
    auto app = App();
    app.start();
}