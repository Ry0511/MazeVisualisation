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
            TextureUnit unit,
            TextureType type
    ) : m_Texture(0),
        m_Unit(unit),
        m_Type(type) {

    }

    Texture2D::Texture2D(
            Texture2D&& other
    ) : m_Texture(other.m_Texture),
        m_Unit(other.m_Unit),
        m_Type(other.m_Type) {
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
        m_Type    = other.m_Type;
        other.m_Texture = 0U;
        return *this;
    }

    //############################################################################//
    // | UTILITY FUNCTIONS |
    //############################################################################//

    void Texture2D::init() {
        ASSERT(m_Texture == 0U, "Texture already initialised...");
        GL(glGenTextures(1, &m_Texture));
        ASSERT(m_Texture != 0U, "Generated Texture is invalid...");
        HINFO("[TEXTURE2D]", " # Initialised Texture '{}'...", m_Texture);
    }

    void Texture2D::bind() {
        assert_init();
        GL(glActiveTexture((GLenum) m_Unit));
        GL(glBindTexture((GLenum) m_Type, m_Texture));
        s_BoundTexture[get_texture_index(m_Unit)] = m_Texture;
    }

    void Texture2D::unbind() {
        assert_init();
        GL(glActiveTexture((GLenum) m_Unit));
        GL(glBindTexture((GLenum) m_Type, 0));
        s_BoundTexture[get_texture_index(m_Unit)] = 0;
    }

    bool Texture2D::is_bound() const {
        return s_BoundTexture[get_texture_index(m_Unit)] == m_Texture;
    }

    void Texture2D::set_texture_image(const Image& image) {
        assert_init();
        ASSERT(
                m_Type == TextureType::TEXTURE_2D,
                "Underlying TextureType is not Texture2D"
        );
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

    void Texture2D::set_texture_images(
            const Image& px,
            const Image& nx,
            const Image& py,
            const Image& ny,
            const Image& pz,
            const Image& nz
    ) {
        assert_init();
        ASSERT(
                m_Type == TextureType::TEXTURE_CUBE,
                "Underlying TextureType is not TextureCube..."
        );
        assert_bound();

        auto map_texture = [](GLenum mapping, const Image& img) {
            GL(
                    glTexImage2D(
                            mapping,
                            0,
                            GL_RGB,
                            img.get_width(),
                            img.get_height(),
                            0,
                            GL_RGB,
                            GL_UNSIGNED_BYTE,
                            (void*) img.get_pixel_data()
                    )
            );
        };

        // Map Cube Images
        map_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, px);
        map_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, nx);
        map_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, py);
        map_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, ny);
        map_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, pz);
        map_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, nz);
    }

    void Texture2D::set_texture_flags(
            std::initializer_list<std::pair<TextureFlag, GLint>> flags
    ) {
        assert_init();
        ASSERT(is_bound(), "Texture must be bound when setting flags...");
        for (const auto [flag, value] : flags) {
            GL(glTexParameteri((GLenum) m_Type, (GLenum) flag, value));
        }
    }

    void Texture2D::enable_preset() {
        using Flag = app::TextureFlag;
        set_texture_flags(
                {
                        std::make_pair(Flag::WRAP_S, GL_REPEAT),
                        std::make_pair(Flag::WRAP_T, GL_REPEAT),
                        std::make_pair(Flag::MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR),
                        std::make_pair(Flag::MAG_FILTER, GL_LINEAR),
                }
        );
    }

    void Texture2D::enable_cubemap_preset() {
        using Flag = app::TextureFlag;
        set_texture_flags(
                {
                        std::make_pair(Flag::MIN_FILTER, GL_LINEAR),
                        std::make_pair(Flag::MAG_FILTER, GL_LINEAR),
                        std::make_pair(Flag::WRAP_S, GL_CLAMP_TO_EDGE),
                        std::make_pair(Flag::WRAP_T, GL_CLAMP_TO_EDGE),
                        std::make_pair(Flag::WRAP_R, GL_CLAMP_TO_EDGE)
                }
        );
    }

} // app