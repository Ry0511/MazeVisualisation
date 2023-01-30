//
// Created by -Ry on 21/01/2023.
//

#ifndef MAZEVISUALISATION_BUFFER_H
#define MAZEVISUALISATION_BUFFER_H

#include "Logging.h"
#include "Renderer/GLUtil.h"
#include <gl/glew.h>
#include <type_traits>
#include <unordered_map>

namespace app {

    //############################################################################//
    // | BUFFER TYPE ENUMERATIONS |
    //############################################################################//

    enum class BufferType : GLuint {
        ARRAY         = GL_ARRAY_BUFFER,
        COPY_READ     = GL_COPY_READ_BUFFER,
        COPY_WRITE    = GL_COPY_WRITE_BUFFER,
        TEXTURE       = GL_TEXTURE_BUFFER,
        UNIFORM       = GL_UNIFORM_BUFFER,
        ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER
    };

    enum class BufferAllocUsage : GLenum {
        STATIC_DRAW  = GL_STATIC_DRAW,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        STREAM_DRAW  = GL_STREAM_DRAW
    };

    template<BufferType T>
    static constexpr GLuint uintof() {
        return static_cast<GLuint>(T);
    }

    template<BufferAllocUsage T>
    static constexpr GLenum uintof() {
        return static_cast<GLenum>(T);
    }

    //############################################################################//
    // | BUFFER CLASS |
    //############################################################################//

    template<
            typename T = float,
            GLuint GLType = GL_FLOAT,
            BufferType Type = BufferType::ARRAY,
            BufferAllocUsage Usage = BufferAllocUsage::STATIC_DRAW,
            size_t TypeSize = sizeof(T)
    > requires (
            std::is_integral<T>().value || std::is_floating_point<T>().value
    )
    class Buffer {

    private:
        inline static std::unordered_map<GLuint, GLuint> s_BoundBuffer{};

    private:
        std::shared_ptr<GLuint> m_BufferHandle;
        bool                    m_IsInitialised;

    public:
        explicit Buffer(
                bool is_late_init = true
        ) : m_BufferHandle(std::make_shared<GLenum>(0)),
            m_IsInitialised(false) {
            if (!is_late_init) init();
        }

        Buffer(
                const Buffer& b
        ) : m_BufferHandle(b.m_BufferHandle),
            m_IsInitialised(b.m_IsInitialised) {
            INFO("{:<20} # {}", "[BUFFER_COPY]", to_string());
        };

        // Default Nature is sharing
        Buffer(Buffer&&) = delete;

        ~Buffer() {
            if (m_IsInitialised && m_BufferHandle.use_count() == 1) {
                GL(glDeleteBuffers(1, m_BufferHandle.get()));
                INFO("{:<20} # {}", "[BUFFER_DELETE]", to_string());
            } else if (m_IsInitialised) {
                INFO("{:<20} # {} # Count={}", "[BUFFER_REF_DEC]", to_string(),
                     m_BufferHandle.use_count() - 1);
            }
        }

    private:
        void assert_initialised() const {
            ASSERT(m_IsInitialised, "Buffer has not been initialised! # {}", to_string());
        }

    public:
        constexpr auto get_gl_type() const { return GLType; }
        constexpr auto get_buffer_type() const { return Type; }
        constexpr auto get_buffer_usage() const { return Usage; }
        constexpr auto get_type_size() const { return TypeSize; }

        bool is_bound() const {
            std::unordered_map<GLuint, GLuint>::iterator pos = s_BoundBuffer.find(uintof<Type>());
            return pos != s_BoundBuffer.end() && (*m_BufferHandle) == (pos->second);
        }

    public:
        std::string to_string() const {
            return std::format(
                    "Handle={:#08x}, Initialised?={}",
                    *m_BufferHandle,
                    m_IsInitialised
            );
        }

        void init() {
            if (!m_IsInitialised) {
                GL(glGenBuffers(1, m_BufferHandle.get()));
                m_IsInitialised = true;

                HINFO("[BUFFER_INIT]", " # {}", to_string());
            } else {
                WARN("Can't initialise an already initialised buffer! # {}", to_string());
            }
        }

        void bind() {
            assert_initialised();
            GL(glBindBuffer(uintof<Type>(), *m_BufferHandle));
            s_BoundBuffer[uintof<Type>()] = *m_BufferHandle;
        }

        void unbind() {
            GL(glBindBuffer(uintof<Type>(), 0));
            s_BoundBuffer[uintof<Type>()] = 0;
        }

        void set_data(T* buffer, size_t count) {
            assert_initialised();
            ASSERT((count * TypeSize) > 0, "Buffer Alloc Size is Zero or less.");

            ASSERT(
                    is_bound(),
                    "Bound buffer is {} which is not this {}.",
                    s_BoundBuffer[uintof<Type>()], *m_BufferHandle
            );

            GL(glBufferData(uintof<Type>(), count * TypeSize, buffer, uintof<Usage>()));
            INFO(
                    "{:<20} # {} # BufferSize: {} ({} * {})",
                    "[BUFFER_DATA]", to_string(), count * TypeSize, count, TypeSize
            );
        }

        [[deprecated("Replaced by VertexArrayObject.h")]]
        void set_attrib_pointer(
                int slot = 0,
                int component_count = 2,
                bool normalise = false,
                GLsizei stride = 2 * TypeSize,
                const GLvoid* ptr = (void*) 0 // nullptr
        ) {
            ASSERT(is_bound(), "Modifying buffer that is not bound!");
            GL(glVertexAttribPointer(
                    slot,
                    component_count,
                    GLType,
                    normalise ? GL_TRUE : GL_FALSE,
                    stride,
                    ptr
            ));
            GL(glEnableVertexAttribArray(slot));
        }

    };

    using StaticIntArrBuffer = Buffer<GLint, GL_INT>;
    using StaticFloatArrBuffer = Buffer<GLfloat, GL_FLOAT>;
    using StaticUIntElementBuffer = Buffer<GLuint, GL_UNSIGNED_INT, BufferType::ELEMENT_ARRAY>;
}

#endif //MAZEVISUALISATION_BUFFER_H
