//
// Header File: Texture2D.cpp
// Date       : 18/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "Texture2D.h"

#include "Logging.h"
#include "GLUtil.h"

namespace app {

    //############################################################################//
    // | CONSTRUCTORS |
    //############################################################################//

    Texture2D::Texture2D(
            TextureUnit unit
    ) : m_Texture(0),
        m_Unit(unit) {
        GL(glGenTextures(1, &m_Texture));
        ASSERT(m_Texture != 0U, "Generated Texture is invalid...");
        HINFO("[TEXTURE2D]", " # Created Texture '{}'...", m_Texture);
    }

    Texture2D::Texture2D(
            Texture2D&& other
    ) : m_Texture(other.m_Texture),
        m_Unit(other.m_Unit) {
        other.m_Texture = 0U;
    }

    Texture2D::~Texture2D() {
        unbind();
        if (m_Texture != 0U) {
            HINFO("[TEXTURE2D]", " # Deleting Texture: '{}'", m_Texture);
            GL(glDeleteTextures(1, &m_Texture));
        }
    }

    //############################################################################//
    // | ASSIGNMENT OPERATOR |
    //############################################################################//

    Texture2D& Texture2D::operator =(Texture2D&& other) {
        if (m_Texture != 0U) {
            GL(glDeleteTextures(1, &m_Texture));
        }
        unbind();
        m_Texture = other.m_Texture;
        m_Unit    = other.m_Unit;
        other.m_Texture = 0U;
        return *this;
    }

    //############################################################################//
    // | UTILITY FUNCTIONS |
    //############################################################################//

    void Texture2D::bind() {
        GL(glActiveTexture((GLenum) m_Unit));
        GL(glBindTexture(GL_TEXTURE_2D, m_Texture));
        s_BoundTexture[get_texture_index(m_Unit)] = m_Texture;
    }

    void Texture2D::unbind() {
        GL(glActiveTexture((GLenum) m_Unit));
        GL(glBindTexture(GL_TEXTURE_2D, 0));
        s_BoundTexture[get_texture_index(m_Unit)] = 0;
    }

    bool Texture2D::is_bound() {
        return s_BoundTexture[get_texture_index(m_Unit)] == m_Texture;
    }

    void Texture2D::set_texture_image(const Image& image) {
        ASSERT(is_bound(), "Texture must be bound before setting data...");
        GL(
                glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RGB,
                        image.get_width(),
                        image.get_height(),
                        0,
                        GL_RGB,
                        GL_UNSIGNED_BYTE,
                        (void*) image.get_pixel_data()
                )
        );
        GL(glGenerateMipmap(GL_TEXTURE_2D));
    }

    void Texture2D::set_texture_flags(
            std::initializer_list<std::pair<TextureFlag, GLint>> flags
    ) {
        ASSERT(is_bound(), "Texture must be bound when setting flags...");
        for (const auto [flag, value] : flags) {
            GL(glTexParameteri(GL_TEXTURE_2D, (GLenum) flag, value));
        }
    }

    void Texture2D::enable_preset() {
        set_texture_flags(
                {
                        std::make_pair(app::TextureFlag::WRAP_S, GL_REPEAT),
                        std::make_pair(app::TextureFlag::WRAP_T, GL_REPEAT),
                        std::make_pair(app::TextureFlag::MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR),
                        std::make_pair(app::TextureFlag::MAG_FILTER, GL_LINEAR),
                }
        );
    }

} // app