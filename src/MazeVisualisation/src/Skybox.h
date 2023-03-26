//
// Header File: Skybox.h
// Date       : 19/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_SKYBOX_H
#define MAZEVISUALISATION_SKYBOX_H

#include "Image.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Shader.h"
#include "Application.h"

namespace maze {

    enum TimeCycle {
        DAY, NIGHT
    };

    class Skybox {

    public:
        inline static const char* s_SkyboxGroup          = "skybox_group";
        inline static const char* s_SkyboxModel          = "Res/Models/TexturedCube.obj";
        inline static const char* s_SkyboxVertexShader   = "Res/Shaders/SkyboxVertexShader.glsl";
        inline static const char* s_SkyboxFragmentShader = "Res/Shaders/SkyboxFragmentShader.glsl";
        inline static const char* s_NightImageResource   = "Res/Textures/Texture-0.png";
        inline static const char* s_DayImageResource     = "Res/Textures/Texture-1.png";

    private:
        TimeCycle      m_CurrentTime;
        app::Texture2D m_DayTexture;
        app::Texture2D m_NightTexture;

    public:
        Skybox(TimeCycle time_cycle = DAY);

    public:
        void init();
        void bind();
        void unbind();
        bool is_bound() const;

        void swap_time();

    public:
        void set_time(TimeCycle time);
        void create_render_group(app::Application* app);
    };

    class SkyboxGroupHandle : public app::GroupHandler {
    private:
        app::Application* m_App;
        Skybox* m_Skybox;

    public:
        SkyboxGroupHandle(app::Application* app, Skybox* skybox);

    public:
        virtual bool is_enabled(app::RenderGroup& group) override;
        virtual bool update(app::RenderGroup& group, app::Vao& vao, app::Shader& shader) override;
    };

} // maze

#endif
