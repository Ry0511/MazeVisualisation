//
// Created by -Ry on 27/01/2023.
//

#ifndef MAZEVISUALISATION_VERTEXARRAYOBJECTREDONE_H
#define MAZEVISUALISATION_VERTEXARRAYOBJECTREDONE_H

#include "Renderer/GLUtil.h"

#include <vector>
#include <array>
#include <gl/glew.h>

namespace app {

    //############################################################################//
    // | ENUMERATION WRAPPING |
    //############################################################################//

    enum class BufferContainerType : GLenum {
        ARRAY          = GL_ARRAY_BUFFER,
        ELEMENT_ARRAY  = GL_ELEMENT_ARRAY_BUFFER,
        TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
        UNIFORM_BUFFER = GL_UNIFORM_BUFFER
    };

    enum class BufferAllocUsage : GLenum {
        STREAM_DRAW  = GL_STREAM_DRAW,
        STATIC_DRAW  = GL_STATIC_DRAW,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW
    };

    enum class GLPrimitiveType : GLenum {
        BYTE   = GL_BYTE,
        UBYTE  = GL_UNSIGNED_BYTE,
        SHORT  = GL_SHORT,
        USHORT = GL_UNSIGNED_SHORT,
        INT    = GL_INT,
        UINT   = GL_UNSIGNED_INT,
        FLOAT  = GL_FLOAT,
        DOUBLE = GL_DOUBLE
    };

    //############################################################################//
    // | BUFFER |
    //############################################################################//

    template<
            class T,
            BufferContainerType BufferType,
            GLPrimitiveType GLType,
            auto TSize = sizeof(T)
    > requires (
            std::is_integral<T>().value || std::is_floating_point<T>().value
    )
    class BufferRedone {

    private:
        inline static std::array<GLuint, 4> s_BoundBufferArray = { 0, 0, 0, 0 };

    private:
        GLuint m_BufferHandle = 0;

    public:
        explicit BufferRedone() = default;

        ~BufferRedone() {
            HINFO("[DELETE_BUFFER]", " # {}", to_string());
            if (is_bound()) unbind();
            if (is_initialised()) GL(glDeleteBuffers(1, &m_BufferHandle));
        }

        BufferRedone(const BufferRedone&) = delete;

        BufferRedone(
                BufferRedone&& b
        ) {
            if (is_bound()) unbind();
            if (is_initialised()) GL(glDeleteBuffers(1, &m_BufferHandle));

            m_BufferHandle = b.m_BufferHandle;
            b.m_BufferHandle = 0;
        }

        BufferRedone& operator =(BufferRedone&& b) {
            if (is_bound()) unbind();
            if (is_initialised()) GL(glDeleteBuffers(1, &m_BufferHandle));

            m_BufferHandle = b.m_BufferHandle;
            b.m_BufferHandle = 0;
            return *this;
        }

    public:

        GLenum get_buffer_type() const {
            return static_cast<GLenum>(BufferType);
        }

        size_t get_buffer_index() const {
            if constexpr (BufferType == BufferContainerType::ARRAY) {
                return 0;
            } else if constexpr (BufferType == BufferContainerType::ELEMENT_ARRAY) {
                return 1;
            }  else if constexpr (BufferType == BufferContainerType::TEXTURE_BUFFER) {
                return 2;
            } else if constexpr (BufferType == BufferContainerType::UNIFORM_BUFFER) {
                return 3;
            }
            throw std::exception("Unknown enumerable...");
        }

        GLenum get_gl_type() const {
            return static_cast<GLenum>(GLType);
        }

        GLuint get_currently_bound() const {
            return s_BoundBufferArray[get_buffer_index()];
        }

        bool is_initialised() const {
            return m_BufferHandle != 0;
        }

        bool is_bound() const {
            return m_BufferHandle != 0 && s_BoundBufferArray[get_buffer_index()] == m_BufferHandle;
        }

        template<bool ShouldBeInitialised>
        constexpr void assert_is_initialised() const {
            if constexpr (ShouldBeInitialised) {
                ASSERT(is_initialised(), "Buffer isn't initialised...");
            } else {
                ASSERT(!is_initialised(), "Buffer is already initialised...");
            }
        }

        template<bool ShouldBeBound>
        constexpr void assert_bound() const {
            if constexpr (ShouldBeBound) {
                ASSERT(is_bound(), "Buffer isn't bound...");
            } else {
                ASSERT(!is_bound(), "Buffer is already bound...");
            }
        }

        std::string to_string() const {
            return std::format(
                    "( BufferHandle: {:#08x}, BufferType: {:#06x}, GLType: {:#06x}, TSize: {} )",
                    m_BufferHandle,
                    get_buffer_type(),
                    get_gl_type(),
                    TSize
            );
        }

        void init() {
            assert_is_initialised<false>();
            GL(glGenVertexArrays(1, &m_BufferHandle));
        }

        void bind() {
            assert_is_initialised<true>();
            GL(glBindBuffer(get_buffer_type(), m_BufferHandle));
            s_BoundBufferArray[get_buffer_index()] = m_BufferHandle;
        }

        void unbind() {
            assert_is_initialised<true>();
            GL(glBindBuffer(get_buffer_type(), 0));
            s_BoundBufferArray[get_buffer_index()] = 0;
        }

        void set_data(
                const T* data,
                size_t count,
                BufferAllocUsage usage = BufferAllocUsage::STATIC_DRAW
        ) {
            assert_is_initialised<true>();
            assert_bound<true>();
            GL(glBufferData(
                    static_cast<GLenum>(BufferType),
                    TSize * count,
                    data,
                    static_cast<GLenum>(usage)
            ));
        }
    };

    using VertexArrBuffer = BufferRedone<GLfloat, BufferContainerType::ARRAY, GLPrimitiveType::FLOAT>;
    using IndexArrBuffer = BufferRedone<GLuint, BufferContainerType::ELEMENT_ARRAY, GLPrimitiveType::UINT>;
    using TextureBuffer = BufferRedone<GLbyte, BufferContainerType::TEXTURE_BUFFER, GLPrimitiveType::BYTE>;

    //############################################################################//
    // | BUFFER ATTRIBUTE |
    //############################################################################//

    template<
            GLPrimitiveType GLType,
            class ActualType,
            auto TypeSize = sizeof(ActualType)
    > requires (
            std::is_integral<ActualType>().value
            || std::is_floating_point<ActualType>().value
    )
    struct AttribPointer {
        GLuint    attrib_index;
        GLint     component_count;
        GLboolean normalise = GL_FALSE;
        GLsizei   stride    = TypeSize * component_count;
        const GLvoid* ptr_offset = (const GLvoid*) 0;

        explicit AttribPointer(
                GLuint attrib_index,
                GLint component_count
        ) : attrib_index(attrib_index),
            component_count(component_count) {
            ASSERT(
                    this->attrib_index >= 0 && this->attrib_index <= 16,
                    "Attribute Index/Slot Ranges from: [0..16]"
            );
            ASSERT(
                    this->component_count > 0 && this->component_count < 5,
                    "Accepted Component Count is: [1, 2, 3, 4]"
            );
        }

        AttribPointer(const AttribPointer&) = default;
        AttribPointer(AttribPointer&&) = default;

        std::string to_string() const {
            return std::format(
                    "( Index: {}, Components: {:#08x}, Normalise?: {}, Stride: {}, Offset: {:p} )",
                    attrib_index,
                    component_count,
                    normalise,
                    stride,
                    ptr_offset
            );
        }

        void create() {
            HINFO("[ATTRIB_CREATE]", " # {}", to_string());
            GL(glVertexAttribPointer(
                    attrib_index,
                    component_count,
                    static_cast<GLenum>(GLType),
                    normalise,
                    stride,
                    ptr_offset
            ));
        }

        void enable() {
            GL(glEnableVertexAttribArray(attrib_index));
        }

        void disable() {
            GL(glEnableVertexAttribArray(attrib_index));
        }

    };

    using FloatAttribPtr = AttribPointer<GLPrimitiveType::FLOAT, GLfloat>;
    using IntAttribPtr = AttribPointer<GLPrimitiveType::INT, GLint>;
    using UIntAttribPtr = AttribPointer<GLPrimitiveType::UINT, GLuint>;
    using ByteAttribPtr = AttribPointer<GLPrimitiveType::BYTE, GLbyte>;

    //############################################################################//
    // | VERTEX ARRAY OBJECT |
    //############################################################################//

    class VertexArrayObjectRedone {

    private:
        inline static GLuint s_CurrentlyBound = 0;

    private:
        GLuint m_VaoHandle = 0;

    public:

        explicit VertexArrayObjectRedone() = default;

        VertexArrayObjectRedone(const VertexArrayObjectRedone&) = delete;

        VertexArrayObjectRedone(VertexArrayObjectRedone&& b) {
            HINFO("[VAO_MOVE]", " # Moving: {:#08x} into {:#08x}", m_VaoHandle, b.m_VaoHandle);
            if (is_initialised()) {
                if (is_bound()) unbind();
                HINFO("[VAO_MOVE_DELETE]", " # Deleting VAO Handle: {:#08x}", m_VaoHandle);
                GL(glDeleteVertexArrays(1, &m_VaoHandle));
            }
            m_VaoHandle = b.m_VaoHandle;
            b.m_VaoHandle = 0;
        }

        ~VertexArrayObjectRedone() {
            if (is_bound()) unbind();
            if (m_VaoHandle != 0) {
                HINFO("[VAO_DELETE]", " # Delete Handle: {:#08x}", m_VaoHandle);
                GL(glDeleteVertexArrays(1, &m_VaoHandle));
            }
        }

        VertexArrayObjectRedone& operator =(VertexArrayObjectRedone&& vao) {
            if (m_VaoHandle != 0) {
                if (is_bound()) unbind();
                GL(glDeleteVertexArrays(1, &m_VaoHandle));
            }

            m_VaoHandle = vao.m_VaoHandle;
            vao.m_VaoHandle = 0;

            return *this;
        }

    public:

        bool is_initialised() const {
            return m_VaoHandle != 0;
        }

        bool is_bound() const {
            return m_VaoHandle != 0 && s_CurrentlyBound == m_VaoHandle;
        }

        template<bool IsInitialised>
        constexpr void assert_init() const {
            if constexpr (IsInitialised) {
                ASSERT(is_initialised(), "VAO hasn't been initialised...");
            } else {
                ASSERT(!is_initialised(), "VAO is initialised...");
            }
        }

        template<bool IsBound>
        constexpr void assert_bound() const {
            if constexpr (IsBound) {
                ASSERT(is_bound(), "Buffer is bound...");
            } else {
                ASSERT(!is_bound(), "Buffer isn't bound...");
            }
        }

        void init() {
            assert_init<false>();
            GL(glGenVertexArrays(1, &m_VaoHandle));
        }

        void bind() {
            assert_init<true>();
            GL(glBindVertexArray(m_VaoHandle));
            s_CurrentlyBound = m_VaoHandle;
        }

        void unbind() {
            assert_init<true>();
            GL(glBindVertexArray(0));
            s_CurrentlyBound = 0;
        }
    };

    //############################################################################//
    // | VERTEX ARRAY OBJECT COMPLETE |
    //############################################################################//

    class IndexedVertexObjectRedo {

    public:
        using VertexBufferVector = std::vector<std::pair<VertexArrBuffer, FloatAttribPtr>>;

    private:
        VertexArrayObjectRedone m_Vao{};
        VertexBufferVector      m_VertexBufferVector{};
        IndexArrBuffer          m_IndexArrBuffer{};

    public:
        IndexedVertexObjectRedo() = default;
        IndexedVertexObjectRedo(const IndexedVertexObjectRedo&) = delete;
        IndexedVertexObjectRedo(IndexedVertexObjectRedo&& ivo) {
            m_Vao                = std::move(ivo.m_Vao);
            m_VertexBufferVector = std::move(ivo.m_VertexBufferVector);
            m_IndexArrBuffer     = std::move(ivo.m_IndexArrBuffer);
        }

        IndexedVertexObjectRedo& operator =(IndexedVertexObjectRedo&& ivo) {
            m_Vao                = std::move(ivo.m_Vao);
            m_VertexBufferVector = std::move(ivo.m_VertexBufferVector);
            m_IndexArrBuffer     = std::move(ivo.m_IndexArrBuffer);
            return *this;
        }

    public:

        void init() {
            m_Vao.init();
            m_IndexArrBuffer.init();
        }

        void bind() {
            m_Vao.bind();

            for (auto&[buffer, attrib] : m_VertexBufferVector) {
                buffer.bind();
                attrib.enable();
            }

            m_IndexArrBuffer.bind();
        }

        void unbind() {
            m_IndexArrBuffer.unbind();
            for (auto& [buffer, attrib] : m_VertexBufferVector) {
                attrib.disable();
                buffer.unbind();
            }
            m_Vao.unbind();
        }

        void add_vertex_buffer(
                float* data,
                size_t count,
                FloatAttribPtr attribute,
                BufferAllocUsage usage = BufferAllocUsage::STATIC_DRAW
        ) {
            ASSERT(m_Vao.is_bound(), "VAO Should be bound when adding Vertex Buffer/s");
            VertexArrBuffer v_buffer{};
            v_buffer.init();
            v_buffer.bind();
            v_buffer.set_data(data, count, usage);
            attribute.create();
            attribute.enable();
            m_VertexBufferVector.emplace_back(
                    std::move(v_buffer), std::move(attribute)
            );
        }

        void set_index_buffer(
                unsigned int* data,
                size_t count,
                BufferAllocUsage usage = BufferAllocUsage::STATIC_DRAW
        ) {
            ASSERT(m_Vao.is_bound(), "VAO Should be bound when setting index buffer data.");
            m_IndexArrBuffer.bind();
            m_IndexArrBuffer.set_data(data, count, usage);
        }
    };

}

#endif //MAZEVISUALISATION_VERTEXARRAYOBJECTREDONE_H
