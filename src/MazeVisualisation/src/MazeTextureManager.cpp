//
// Header File: MazeTextureManager.cpp
// Date       : 28/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "MazeTextureManager.h"
#include "Image.h"

namespace maze {

    MazeTextureManager::MazeTextureManager(

    ) : m_ActiveTexture(0),
        m_Textures() {

    }

    void MazeTextureManager::init() {
        for (size_t i = 0; i < m_Textures.size(); ++i) {
            auto& texture = m_Textures[i];
            const auto& path = s_TexturePaths[i];

            app::Image img{path};

            texture.init();
            texture.bind();
            texture.set_texture_image(img);
            texture.enable_preset();
            texture.unbind();
        }
        bind();
    }

    void MazeTextureManager::bind() {
        // 0 Check is redundant since its unsigned
        ASSERT(m_ActiveTexture < s_TextureCount, "Texture count out of range.");
        m_Textures[m_ActiveTexture].bind();
    }

    void MazeTextureManager::unbind() {
        // 0 Check is redundant since its unsigned
        ASSERT(m_ActiveTexture < s_TextureCount, "Texture count out of range.");
        m_Textures[m_ActiveTexture].unbind();
    }

    void MazeTextureManager::update(app::Application* app, float delta) {
        if (app->is_key_down(app::Key::F)) {
            unbind();

            // Cycle Active Texture
            m_ActiveTexture++;
            if (m_ActiveTexture >= s_TextureCount) {
                m_ActiveTexture = 0;
            }

            bind();
        }
    }

} // maze