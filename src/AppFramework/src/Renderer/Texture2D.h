//
// Header File: Texture2D.h
// Date       : 18/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_TEXTURE2D_H
#define MAZEVISUALISATION_TEXTURE2D_H

#include "Image.h"
#include "Logging.h"

#include <gl/glew.h>
#include <initializer_list>
#include <utility>
#include <array>

namespace app {

    //############################################################################//
    // | GLEW TEXTURE 2D ENUMERATIONS |
    //############################################################################//

    enum class TextureType {
        TEXTURE_2D   = GL_TEXTURE_2D,
        TEXTURE_CUBE = GL_TEXTURE_CUBE_MAP
    };

    // See: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
    // for potential values
    enum class TextureFlag : GLenum {
        BASE_LEVEL   = GL_TEXTURE_BASE_LEVEL,
        COMPARE_FUNC = GL_TEXTURE_COMPARE_FUNC,
        COMPARE_MODE = GL_TEXTURE_COMPARE_MODE,
        LOD_BIAS     = GL_TEXTURE_LOD_BIAS,
        MIN_LOD      = GL_TEXTURE_MIN_LOD,
        MAX_LOD      = GL_TEXTURE_MAX_LOD,
        WRAP_S       = GL_TEXTURE_WRAP_S,
        WRAP_R       = GL_TEXTURE_WRAP_R,
        WRAP_T       = GL_TEXTURE_WRAP_T,
        MIN_FILTER   = GL_TEXTURE_MIN_FILTER,
        MAG_FILTER   = GL_TEXTURE_MAG_FILTER
    };

    enum class TextureUnit : GLenum {
        ZERO  = GL_TEXTURE0,
        ONE   = GL_TEXTURE1,
        TWO   = GL_TEXTURE2,
        THREE = GL_TEXTURE3,
        FOUR  = GL_TEXTURE4,
        FIVE  = GL_TEXTURE5,
        SIX   = GL_TEXTURE6,
        SEVEN = GL_TEXTURE7,
        EIGHT = GL_TEXTURE8,
        NINE  = GL_TEXTURE9,
        TEN   = GL_TEXTURE10
    };

    inline static constexpr GLenum s_TextureOffset    = (GLenum) TextureUnit::ZERO;
    inline static constexpr size_t s_TextureUnitCount = 10;

    static_assert((GLenum) TextureUnit::ZERO - s_TextureOffset == 0, "");
    static_assert((GLenum) TextureUnit::ONE - s_TextureOffset == 1, "");
    static_assert((GLenum) TextureUnit::TWO - s_TextureOffset == 2, "");
    static_assert((GLenum) TextureUnit::THREE - s_TextureOffset == 3, "");
    static_assert((GLenum) TextureUnit::FOUR - s_TextureOffset == 4, "");
    static_assert((GLenum) TextureUnit::FIVE - s_TextureOffset == 5, "");
    static_assert((GLenum) TextureUnit::SIX - s_TextureOffset == 6, "");
    static_assert((GLenum) TextureUnit::SEVEN - s_TextureOffset == 7, "");
    static_assert((GLenum) TextureUnit::EIGHT - s_TextureOffset == 8, "");
    static_assert((GLenum) TextureUnit::NINE - s_TextureOffset == 9, "");
    static_assert((GLenum) TextureUnit::TEN - s_TextureOffset == 10, "");

    inline static constexpr size_t get_texture_index(TextureUnit unit) {
        return ((GLenum) unit) - s_TextureOffset;
    }

    //############################################################################//
    // | TEXTURE WRAPPER CLASS |
    //############################################################################//

    class Texture2D {

    private:
        inline static std::array<unsigned int, s_TextureUnitCount> s_BoundTexture{ 0U };

    private:
        unsigned int m_Texture;
        TextureUnit  m_Unit;
        TextureType  m_Type;

    public:
        Texture2D(TextureUnit unit = TextureUnit::ZERO, TextureType type = TextureType::TEXTURE_2D);
        Texture2D(const Texture2D&) = delete;
        Texture2D(Texture2D&&);

    public:
        Texture2D& operator =(const Texture2D&) = delete;
        Texture2D& operator =(Texture2D&&);
        ~Texture2D();

    public:
        unsigned int get_texture_id() const {
            return m_Texture;
        }

        TextureUnit get_texture_unit() const {
            return m_Unit;
        }

        void set_texture_unit(TextureUnit unit) {
            m_Unit = unit;
        }

    public:
        void init();
        void bind();
        void unbind();
        bool is_bound() const;

    public:
        void set_texture_image(const Image& image);
        void set_texture_images(
                const Image& px, const Image& nx,
                const Image& py, const Image& ny,
                const Image& pz, const Image& nz
        );
        void set_texture_flags(std::initializer_list<std::pair<TextureFlag, GLint>> flags);
        void enable_preset();
        void enable_cubemap_preset();

    public:
        void assert_init() const {
            ASSERT(m_Texture != 0U, "Texture has not been initialised...");
        }

        void assert_bound() const {
            ASSERT(is_bound(), "Texture should be bound...");
        }
    };

} // app

#endif
