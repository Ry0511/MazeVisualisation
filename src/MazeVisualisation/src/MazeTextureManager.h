//
// Header File: MazeTextureManager.h
// Date       : 28/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_MAZETEXTUREMANAGER_H
#define MAZEVISUALISATION_MAZETEXTUREMANAGER_H

#include "Renderer/Texture2D.h"
#include "Application.h"

namespace maze {

    class MazeTextureManager {

    private:
        static constexpr size_t s_TextureCount = 5;
        inline static const std::array<std::string, s_TextureCount> s_TexturePaths{
            "Res/Textures/Texture-2.png",
            "Res/Textures/Texture-3.png",
            "Res/Textures/Texture-4.png",
            "Res/Textures/Texture-5.png",
            "Res/Textures/Texture-6.png"
        };

    private:
        std::array<app::Texture2D, s_TextureCount> m_Textures;
        size_t m_ActiveTexture;

    public:
        MazeTextureManager();

    public:
        void init();
        void bind();
        void unbind();
        void update(app::Application* app, float delta);

    };

} // maze

#endif
