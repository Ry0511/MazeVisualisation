//
// Created by -Ry on 16/01/2023.
//

#include "Application.h"
#include "MazeManager.h"
#include "Logging.h"
#include "Renderer/GLUtil.h"
#include "Skybox.h"

class App : public app::Application {

private:
    maze::MazeManager m_MazeManager{};
    maze::Skybox      m_Skybox{ maze::TimeCycle::DAY };

public:
    App() : app::Application("My App", 800, 600) {}

    virtual void camera_update(app::Window& window, float delta) override {
        Camera3D::camera_update(window, delta);
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        GL(glLineWidth(1));

        m_MazeManager.init(this);
        m_Skybox.init();
        m_Skybox.create_render_group(this);
    }

    virtual bool on_update(float delta) override {

        const auto& cam = get_camera_state();
        set_title(
                std::format(
                        "Window # {:#<4} fps,"
                        " Delta: {:#<2.2f} ms,"
                        " Pos: {:#<2.2f}, {:#<2.2f}, {:#<2.2f}"
                        " PosNormalised: {:#<2.2f}, {:#<2.2f}, {:#<2.2f}",
                        (int) (1.0 / (delta)),
                        delta * 1000.F,

                        // Actual Cam Pos
                        cam.cam_pos.x,
                        cam.cam_pos.y,
                        cam.cam_pos.z,

                        // Normalised Grid Position
                        cam.cam_pos.x / 2.5F,
                        cam.cam_pos.y,
                        cam.cam_pos.z / 2.5F
                ).c_str()
        );

        if (is_key_down(app::Key::T)) {
            m_Skybox.swap_time();
        }

        // Pre-Update
        Renderer::clear();
        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);

        // Update & Render
        m_MazeManager.update(this, delta);
        this->Renderer::update_groups(delta);
        this->Renderer::render_groups();

        return true;
    }
};

int main() {
    App app{};
    app.start();
}