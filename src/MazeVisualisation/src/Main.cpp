//
// Created by -Ry on 16/01/2023.
//

#include "Application.h"

#include "MazeManager.h"

#include "MazeConstructs.h"
#include "Logging.h"
#include "Renderer/GLUtil.h"

class App : public app::Application {

private:
    float             m_Theta     = 0.0F;
    size_t            m_TickCount = 0;
    maze::MazeManager m_MazeManager{};

public:
    App() : app::Application("My App", 800, 600) {}

    virtual void camera_update(app::Window& window, float delta) override {
        Camera3D::camera_update(window, delta);
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        set_clear_colour({ 0.1, 0.1, 0.1, 1.0 });
        GL(glEnable(GL_STENCIL_TEST));
        GL(glEnable(GL_DEPTH_TEST));
        GL(glEnable(GL_CULL_FACE));
        GL(glEnable(GL_MULTISAMPLE));
        GL(glFrontFace(GL_CCW));
        GL(glCullFace(GL_BACK));
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(1));

        m_MazeManager.init(this);
    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;

        Renderer::clear();

        set_title(
                std::format(
                        "Window # {:<4} fps, Delta: {:<2.2f} ms",
                        (int) (1.0 / (delta)),
                        delta * 1000.F
                ).c_str()
        );

        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);

        m_MazeManager.update(this, delta);
        this->Renderer::update_and_render_groups(delta);

        return true;
    }
};

int main() {
    auto app = new App();
    app->start();
    delete app;
}