//
// Created by -Ry on 24/01/2023.
//

#ifndef MAZEVISUALISATION_VERTEXARRAYOBJECT_H
#define MAZEVISUALISATION_VERTEXARRAYOBJECT_H

#include "Buffer.h"

#include <gl/glew.h>
#include <format>
#include <memory.h>
#include <string>
#include <type_traits>

namespace app {

    //############################################################################//
    // | ENUMERATION WRAPPING |
    //############################################################################//

    enum class PrimitiveType : GLenum {
        BYTE   = GL_BYTE,
        UBYTE  = GL_UNSIGNED_BYTE,
        SHORT  = GL_SHORT,
        USHORT = GL_UNSIGNED_SHORT,
        INT    = GL_INT,
        UINT   = GL_UNSIGNED_INT,
        FLOAT  = GL_FLOAT,
        DOUBLE = GL_DOUBLE
    };

    enum class ComponentCount : GLint {
        ONE   = 1, // Congrats
        TWO   = 2, // You
        THREE = 3, // Can
        FOUR  = 4, // Count!
    };

    //############################################################################//
    // | VERTEX ATTRIB POINTER |
    //############################################################################//

    template<
            GLuint Index,
            ComponentCount Size,
            PrimitiveType PrimType,
            typename LitType,
            GLboolean Normalised = GL_FALSE,
            const GLvoid* Pointer = (const GLvoid*) 0,
            GLsizei Stride = sizeof(LitType) * static_cast<GLint>(Size)
    > requires (
            std::is_integral<LitType>().value
            || std::is_floating_point<LitType>().value
    )
    struct AttribPointer {

        std::string to_string() const {
            return std::format(
                    "Index: {}, ComponentCount: {}, "
                    "Type: {:#08x}, IsNormalised?: {}, "
                    "Ptr: {:p}, Stride: {}",
                    Index,
                    static_cast<GLint>(Size),
                    static_cast<GLenum>(PrimType),
                    Normalised,
                    (const void*) Pointer,
                    Stride
            );
        }

        void create() const {
            INFO("{:<20} # Index: {}", "[ATTRIB_CREATE]", Index);
            GL(glVertexAttribPointer(
                    Index,
                    static_cast<GLint>(Size),
                    static_cast<GLenum>(PrimType),
                    Normalised,
                    Stride,
                    Pointer
            ));
        }

        void enable() {
            GL(glEnableVertexAttribArray(Index));
        }

        void disable() {
            GL(glDisableVertexAttribArray(Index));
        }
    };

    //############################################################################//
    // | DEFAULT ATTRIBUTE SPECIALISATIONS |
    //############################################################################//

    #define INDEXED template<GLuint Index> requires (Index < 16)

    // Float Attributes
    template<GLuint Index, ComponentCount Size>
    using FloatAttribPointer = AttribPointer<Index, Size, PrimitiveType::FLOAT, GLfloat>;
    INDEXED using FloatAttribPointer1D = FloatAttribPointer<Index, ComponentCount::ONE>;
    INDEXED using FloatAttribPointer2D = FloatAttribPointer<Index, ComponentCount::TWO>;
    INDEXED using FloatAttribPointer3D = FloatAttribPointer<Index, ComponentCount::THREE>;
    INDEXED using FloatAttribPointer4D = FloatAttribPointer<Index, ComponentCount::FOUR>;

    // Int Attributes
    template<GLuint Index, ComponentCount Size>
    using IntAttribPointer = AttribPointer<Index, Size, PrimitiveType::INT, GLint>;
    INDEXED using IntAttribPointer1D = IntAttribPointer<Index, ComponentCount::ONE>;
    INDEXED using IntAttribPointer2D = IntAttribPointer<Index, ComponentCount::TWO>;
    INDEXED using IntAttribPointer3D = IntAttribPointer<Index, ComponentCount::THREE>;
    INDEXED using IntAttribPointer4D = IntAttribPointer<Index, ComponentCount::FOUR>;

    //############################################################################//
    // | VERTEX ARRAY OBJECT (VAO) |
    //############################################################################//

    class VertexArrayObject {

    private:
        inline static GLuint s_CurrentlyBound = 0;

    private:
        std::shared_ptr<GLuint> m_Handle;
        bool                    m_IsInitialised = false;

    public:

        explicit VertexArrayObject(
                bool is_late_init = true
        ) : m_Handle(std::make_shared<GLuint>()) {
            if (!is_late_init) {
                init();
            }
        }

        VertexArrayObject(
                const VertexArrayObject& vao
        ) : m_Handle(vao.m_Handle),
            m_IsInitialised(vao.m_IsInitialised) {
            INFO("{:<20} # {}", "[VAO_COPY]", to_string());
        }

        VertexArrayObject(VertexArrayObject&&) = delete;

        ~VertexArrayObject() {
            if (m_IsInitialised && m_Handle.use_count() == 1) {
                GL(glDeleteVertexArrays(1, m_Handle.get()));
                INFO("{:<20} # {}", "[DELETE_VAO]", to_string());
            }
        }

    private:
        void assert_initialised() const {
            ASSERT(m_IsInitialised, "VAO Has not been initialised. # {}", to_string());
        }

    public:
        std::string to_string() const {
            return std::format(
                    "Handle: {:p}, IsInitialised?: {}",
                    (const void*) m_Handle.get(),
                    m_IsInitialised
            );
        }

        void init() {
            if (!(m_IsInitialised)) {
                GL(glGenVertexArrays(1, m_Handle.get()));
                m_IsInitialised = true;
                INFO("{:<20} # {}", "[VAO_INIT]", to_string());
            } else {
                WARN("VAO Has already been initialised!");
            }
        }

        bool is_bound() const {
            return m_IsInitialised && s_CurrentlyBound == (*m_Handle);
        }

        void bind() {
            assert_initialised();
            GL(glBindVertexArray(*m_Handle));
            s_CurrentlyBound = *m_Handle;
        }

        void unbind() const {
            GL(glBindVertexArray(0));
            s_CurrentlyBound = 0;
        }
    };

}

#endif //MAZEVISUALISATION_VERTEXARRAYOBJECT_H
