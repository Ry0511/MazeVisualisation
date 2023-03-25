//
// Header File: Skybox.cpp
// Date       : 19/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "Skybox.h"
#include "Logging.h"
#include "CommonModelFileReaders.h"

namespace maze {

    Skybox::Skybox(
            TimeCycle time_cycle
    ) : m_CurrentTime(time_cycle),
        m_DayTexture(app::TextureUnit::ZERO, app::TextureType::TEXTURE_CUBE),
        m_NightTexture(app::TextureUnit::ZERO, app::TextureType::TEXTURE_CUBE) {
        HINFO("[SKYBOX]", " # Created...");
    }

    void Skybox::init() {
        HINFO("[SKYBOX]", " # Initialising Skybox...");

        // Day Image Texture
        m_DayTexture.init();
        app::Image day_image{ s_DayImageResource };
        m_DayTexture.bind();
        m_DayTexture.set_texture_images(
                day_image, day_image, day_image,
                day_image, day_image, day_image
        );
        m_DayTexture.enable_cubemap_preset();
        m_DayTexture.unbind();

        // Night Image Texture
        m_NightTexture.init();
        app::Image night_image{ s_NightImageResource };
        m_NightTexture.bind();
        m_NightTexture.set_texture_images(
                night_image, night_image, night_image,
                night_image, night_image, night_image
        );
        m_NightTexture.enable_cubemap_preset();
        m_NightTexture.unbind();

        HINFO("[SKYBOX]", " # Initialising Finished...");
    }

    void Skybox::bind() {
        switch (m_CurrentTime) {
            case DAY: {
                m_DayTexture.bind();
                break;
            }
            case NIGHT: {
                m_NightTexture.bind();
                break;
            }
        }
    }

    void Skybox::unbind() {
        switch (m_CurrentTime) {
            case DAY: {
                m_DayTexture.unbind();
                break;
            }
            case NIGHT: {
                m_NightTexture.unbind();
                break;
            }
        }
    }

    bool Skybox::is_bound() const {
        return (m_DayTexture.is_bound() || m_NightTexture.is_bound());
    }

    void Skybox::set_time(TimeCycle time) {
        m_CurrentTime = time;
    }

    void Skybox::create_render_group(app::Application* app) {

        app::Mutable3DModel model{};
        app::model_file::read_wavefront_file(s_SkyboxModel, model);
        app->create_render_group(
                s_SkyboxGroup,
                std::move(model),
                s_SkyboxVertexShader,
                s_SkyboxFragmentShader
        );

        app::RenderGroup& group = app->get_group(s_SkyboxGroup);
        group.add_group_handler<SkyboxGroupHandle>(app, this);

        group.set_on_bind([](auto&){
            GL(glDepthFunc(GL_LEQUAL));
        });

        group.set_on_unbind([](auto&){
            GL(glDepthFunc(GL_LESS));
        });

        app::Entity skybox_entity;
        skybox_entity.set_dirty();
        group.queue_entity(std::move(skybox_entity));
    }

    //############################################################################//
    // | SKYBOX GROUP HANDLER |
    //############################################################################//

    SkyboxGroupHandle::SkyboxGroupHandle(
            app::Application* app,
            Skybox* skybox
    ) : m_App(app),
        m_Skybox(skybox) {

    }

    bool SkyboxGroupHandle::is_enabled(
            app::RenderGroup& group
    ) {
        return m_App != nullptr;
    }

    bool SkyboxGroupHandle::update(
            app::RenderGroup& group,
            app::Vao& vao,
            app::Shader& shader
    ) {
        glm::mat4 view = glm::mat4{ glm::mat3{ m_App->get_camera_matrix() }};
        shader.set_uniform(app::Shader::s_ProjectionMatrixUniform, m_App->get_proj_matrix());
        shader.set_uniform(app::Shader::s_ViewMatrixUniform, view);
        m_Skybox->bind();
        return true;
    }

} // maze