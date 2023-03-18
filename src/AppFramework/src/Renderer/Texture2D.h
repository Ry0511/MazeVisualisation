//
// Header File: Texture2D.h
// Date       : 18/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_TEXTURE2D_H
#define MAZEVISUALISATION_TEXTURE2D_H

#include "Image.h"

#include <gl/glew.h>
#include <initializer_list>
#include <utility>
#include <array>

namespace app {

    //############################################################################//
    // | GLEW TEXTURE 2D ENUMERATIONS |
    //############################################################################//

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

    inline static constexpr size_t s_TextureUnitCount = 10;

    //############################################################################//
    // | TEXTURE WRAPPER CLASS |
    //############################################################################//

    class Texture2D {

    private:
        inline static std::array<unsigned int, s_TextureUnitCount> s_BoundTexture{ 0U };

    private:
        unsigned int m_Texture;

    public:
        Texture2D();
        Texture2D(const Texture2D&) = delete;
        Texture2D(Texture2D&&);

    public:
        Texture2D& operator =(const Texture2D&) = delete;
        Texture2D& operator =(Texture2D&&);

    public:
        void bind(TextureUnit unit = TextureUnit::ZERO);
        void unbind(TextureUnit unit = TextureUnit::ZERO);
        bool is_bound(TextureUnit unit = TextureUnit::ZERO);

    public:
        void set_texture_image(const Image& image);
        void set_texture_flags(std::initializer_list<std::pair<TextureFlag, GLint>> flags);
    };

} // app

#endif
