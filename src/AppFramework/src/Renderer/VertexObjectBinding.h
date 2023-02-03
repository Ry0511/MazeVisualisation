//
// Created by -Ry on 03/02/2023.
//

#ifndef MAZEVISUALISATION_VERTEXOBJECTBINDING_H
#define MAZEVISUALISATION_VERTEXOBJECTBINDING_H

#include "Renderer/GLUtil.h"

#include <vector>
#include <array>
#include <type_traits>
#include <gl/glew.h>

namespace app::redone {

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

    template<class>
    inline constexpr bool always_false = false;

    template<class T>
    static constexpr PrimitiveType deduce_prim_type() {
        if constexpr (std::is_same<GLfloat, T>()) {
            return PrimitiveType::FLOAT;

        } else if constexpr (std::is_same<GLint, T>()) {
            return PrimitiveType::INT;

        } else if constexpr (std::is_same<GLuint, T>()) {
            return PrimitiveType::UINT;

        } else if constexpr (std::is_same<GLdouble, T>()) {
            return PrimitiveType::DOUBLE;

        } else if constexpr (std::is_same<GLbyte, T>()) {
            return PrimitiveType::BYTE;

        } else if constexpr (std::is_same<GLubyte, T>()) {
            return PrimitiveType::UBYTE;

        } else if constexpr (std::is_same<GLshort, T>()) {
            return PrimitiveType::SHORT;

        } else if constexpr (std::is_same<GLushort, T>()) {
            return PrimitiveType::USHORT;

        } else {
            static_assert(always_false<T>, "Unknown Primitive Type Provided.");
        }
    }

    //############################################################################//
    // | TEMPLATES |
    //############################################################################//

    class DelayedInit {
    public:
        virtual bool is_init() = 0;
        virtual void init() = 0;
    };

    class Bindable {
    public:
        virtual void bind() = 0;
        virtual bool is_bound() = 0;
        virtual void unbind() = 0;
    };

    //############################################################################//
    // | ABSTRACT BUFFER |
    //############################################################################//

    struct BufferState {
        GLuint              handle         = 0;
        BufferContainerType container_type = BufferContainerType::ARRAY;

        std::string to_string() const {
            return std::format(
                    "( Handle: {:#08x}, Container: {:#08x} )",
                    handle,
                    static_cast<GLenum>(container_type)
            );
        }

        ~BufferState() {
            if (handle != 0) {
                HINFO("[BUFFER_STATE]", " # Deleting: {}", to_string());
                GL(glDeleteBuffers(1, &handle));
            }
        }

        GLenum get_buffer_type() const {
            return static_cast<GLenum>(container_type);
        }

        GLuint get_buffer_index() const {
            switch (container_type) {
                case BufferContainerType::ARRAY: {
                    return 0;
                }
                case BufferContainerType::TEXTURE_BUFFER: {
                    return 1;
                }
                case BufferContainerType::ELEMENT_ARRAY: {
                    return 2;
                }
                case BufferContainerType::UNIFORM_BUFFER: {
                    return 3;
                }
            }
            HERR("[UNKNOWN_ENUMERABLE]", " # Provided enumerable can't be deduced.");
            throw std::exception();
        }
    };

    using SimpleBufferState = std::shared_ptr<BufferState>;

    class SimpleBuffer : public DelayedInit, public Bindable {

    private:
        static inline std::array<GLenum, 4> s_BoundBuffer{ 0 };

    private:
        SimpleBufferState m_BufferState = std::make_shared<BufferState>();

    public:
        SimpleBuffer(BufferContainerType type) {
            m_BufferState->container_type = type;
        }

    public:
        virtual bool is_init() override {
            return m_BufferState->handle != 0;
        }

        virtual void init() override {
            ASSERT(!is_init(), "Buffer already initialised...");
            GL(glGenBuffers(1, &m_BufferState->handle));
        }

        virtual void bind() override {
            ASSERT(is_init(), "Buffer hasn't been initialised...");
            GL(glBindBuffer(m_BufferState->get_buffer_type(), m_BufferState->handle));
            s_BoundBuffer[m_BufferState->get_buffer_index()] = m_BufferState->handle;
        }

        virtual bool is_bound() override {
            return s_BoundBuffer[m_BufferState->get_buffer_index()] == m_BufferState->handle;
        }

        virtual void unbind() override {
            ASSERT(is_init(), "Buffer hasn't been initialised...");
            GL(glBindBuffer(m_BufferState->get_buffer_type(), 0));
            s_BoundBuffer[m_BufferState->get_buffer_index()] = 0;
        }

        //############################################################################//
        // | SETTING BUFFER DATA |
        //############################################################################//

        template<class T,
                BufferAllocUsage Usage,
                auto TSize = sizeof(T),
                auto UsageValue = static_cast<GLenum>(Usage)
        >
        void set_data(const void* data, size_t count) {
            ASSERT(is_init(), "Buffer hasn't been initialised...");
            ASSERT(is_bound(), "Buffer isn't bound...");

            HINFO(
                    "[SET_DATA]", " # {:#08x}, {}, {:p}, {:#08x}",
                    m_BufferState->get_buffer_type(),
                    TSize * count,
                    data,
                    UsageValue
            );
            GL(glBufferData(
                    m_BufferState->get_buffer_type(),
                    TSize * count,
                    data,
                    UsageValue
            ));
        }

        template<class T>
        void set_data_static(const void* data, size_t count) {
            set_data<T, BufferAllocUsage::STATIC_DRAW>(data, count);
        }

        template<class T>
        void set_data_dynamic(const void* data, size_t count) {
            set_data<T, BufferAllocUsage::DYNAMIC_DRAW>(data, count);
        }

        template<class T>
        void set_data_stream(const void* data, size_t count) {
            set_data<T, BufferAllocUsage::STREAM_DRAW>(data, count);
        }
    };

    static SimpleBuffer array_buffer() {
        return SimpleBuffer{ BufferContainerType::ARRAY };
    }

    static SimpleBuffer elem_array_buffer() {
        return SimpleBuffer{ BufferContainerType::ELEMENT_ARRAY };
    }

    static SimpleBuffer tex_buffer() {
        return SimpleBuffer{ BufferContainerType::TEXTURE_BUFFER };
    }

    static SimpleBuffer uniform_buffer() {
        return SimpleBuffer{ BufferContainerType::UNIFORM_BUFFER };
    }

    //############################################################################//
    // | VERTEX ATTRIBUTE |
    //############################################################################//

    struct AbstractAttributePointer {

    protected:
        GLint index;

    public:
        AbstractAttributePointer(const GLint index) : index(index) {}
        virtual ~AbstractAttributePointer() = default;

    public:
        virtual void create() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;

        virtual void create_and_enable() {
            create();
            enable();
        }
    };

    template<
            GLint ComponentCount,
            class T,
            PrimitiveType GLType,
            auto GLTypeValue = static_cast<GLenum>(GLType),
            auto TSize = sizeof(T)
    >
    struct ComponentRestrainedAttribute : public AbstractAttributePointer {

        GLint     divisor;
        GLboolean normalise;
        GLsizei   stride;
        const GLvoid* offset;

        ComponentRestrainedAttribute(
                GLuint index,
                GLint divisor = 0,
                GLboolean normalise = GL_FALSE,
                GLsizei stride = 0,
                const GLvoid* offset = 0
        ) : AbstractAttributePointer(index),
            divisor(divisor),
            normalise(normalise),
            stride(stride == 0 ? TSize * ComponentCount : stride),
            offset(offset) {}

        std::string to_string() const {
            return std::format(
                    "( {}, {}, {}, {}, {}, {:p} )",
                    index,
                    ComponentCount,
                    divisor,
                    normalise == GL_TRUE,
                    stride,
                    offset
            );
        }

        virtual void create() override {
            HINFO("[CRA_CREATE]", " # {}", to_string());
            GL(glVertexAttribPointer(
                    index,
                    ComponentCount,
                    GLTypeValue,
                    normalise,
                    stride,
                    offset
            ));
        }

        virtual void enable() override {
            GL(glEnableVertexAttribArray(index));
            GL(glVertexAttribDivisor(index, divisor));
        }

        virtual void disable() override {
            GL(glDisableVertexAttribArray(index));
            GL(glVertexAttribDivisor(index, 0));
        }
    };

    using Vec2Attribute = ComponentRestrainedAttribute<2, GLfloat, PrimitiveType::FLOAT>;
    using Vec3Attribute = ComponentRestrainedAttribute<3, GLfloat, PrimitiveType::FLOAT>;
    using IntVec2Attribute = ComponentRestrainedAttribute<2, GLint, PrimitiveType::INT>;
    using IntVec3Attribute = ComponentRestrainedAttribute<3, GLint, PrimitiveType::INT>;
    using UIntVec2Attribute = ComponentRestrainedAttribute<2, GLuint, PrimitiveType::UINT>;
    using UIntVec3Attribute = ComponentRestrainedAttribute<3, GLuint, PrimitiveType::UINT>;
    using ByteVec2Attribute = ComponentRestrainedAttribute<2, GLbyte, PrimitiveType::BYTE>;
    using ByteVec3Attribute = ComponentRestrainedAttribute<3, GLbyte, PrimitiveType::BYTE>;

    //############################################################################//
    // | MATRIX BASED ATTRIBUTES |
    //############################################################################//

    template<
            GLuint RowCount,
            GLuint ColCount,
            class T,
            PrimitiveType GLType,
            auto GLTypeValue = static_cast<GLenum>(GLType),
            auto TSize = sizeof(T),
            auto RowSize = TSize * ColCount,
            auto Stride = RowSize * RowCount
    >
    struct MatrixAttributePointer : public AbstractAttributePointer {
        GLint     divisor;
        GLboolean normalise;

        MatrixAttributePointer(
                GLuint index,
                GLint divisor = 1,
                GLboolean normalise = GL_FALSE
        ) : AbstractAttributePointer(index),
            divisor(divisor),
            normalise(normalise) {
        }

        std::string to_string() const {
            return std::format(
                    "( {}, {}, {}, {}, {} )",
                    index,
                    divisor,
                    normalise == GL_TRUE,
                    RowSize,
                    Stride
            );
        }

        virtual void create() override {
            HINFO("[MAP_CREATE]", " # {}", to_string());

            for (GLuint i = 0; i < RowCount; ++i) {
                GLuint pos = index + i;
                GL(glEnableVertexAttribArray(pos));
                GL(glVertexAttribDivisor(pos, divisor));
                GL(glVertexAttribPointer(
                        pos,
                        ColCount,
                        GLTypeValue,
                        normalise,
                        sizeof(glm::mat4),
                        (const GLvoid*) (i * RowSize)
                ));
            }

        }

        virtual void enable() override {
            for (GLuint i = 0; i < RowCount; ++i) {
                GL(glEnableVertexAttribArray(index + i));
                GL(glVertexAttribDivisor(index + i, divisor));
            }
        }

        virtual void disable() override {
            for (GLuint i = 0; i < RowCount; ++i) {
                GL(glDisableVertexAttribArray(index + i));
                GL(glVertexAttribDivisor(index + i, 0));
            }
        }
    };

    using FloatMat4Attrib = MatrixAttributePointer<4, 4, GLfloat, PrimitiveType::FLOAT>;
    using IntMat4Attrib = MatrixAttributePointer<4, 4, GLint, PrimitiveType::INT>;
    using ByteMat4Attrib = MatrixAttributePointer<4, 4, GLbyte, PrimitiveType::BYTE>;

    //############################################################################//
    // | VERTEX ARRAY OBJECT / VAO |
    //############################################################################//

    using AttributeVector = std::vector<std::unique_ptr<AbstractAttributePointer>>;

    struct VaoState {
        GLuint                    handle = 0;
        SimpleBuffer              index_buffer{ BufferContainerType::ELEMENT_ARRAY };
        std::vector<SimpleBuffer> buffers{};
        AttributeVector           attributes{};
    };

    using VaoStateHandle = std::shared_ptr<VaoState>;

    class Vao : public DelayedInit, public Bindable {

    private:
        inline static GLuint s_CurrentlyBound = 0;

    private:
        VaoStateHandle m_State = std::make_shared<VaoState>();

    public:
        virtual bool is_init() override {
            return m_State->handle != 0;
        }

        virtual void init() override {
            ASSERT(!is_init(), "VAO already initialised...");
            GL(glGenVertexArrays(1, &m_State->handle));
            m_State->index_buffer.init();
            HINFO("[VAO_INIT]", " # VAO: {:#08x}", m_State->handle);
        }

        virtual void bind() override {
            ASSERT(is_init(), "VAO not initialised...");
            GL(glBindVertexArray(m_State->handle));
            s_CurrentlyBound = m_State->handle;
            m_State->index_buffer.bind();
        }

        virtual void bind_all() {
            bind();

            for (size_t i = 0; i < m_State->buffers.size(); ++i) {
                m_State->buffers[i].bind();
                m_State->attributes[i]->enable();
            }
        }

        virtual bool is_bound() override {
            return s_CurrentlyBound == m_State->handle;
        }

        virtual void unbind() override {
            ASSERT(is_init(), "VAO not initialised...");
            for (size_t i = 0; i < m_State->buffers.size(); ++i) {
                m_State->attributes[i]->disable();
                m_State->buffers[i].unbind();
            }
            GL(glBindVertexArray(0));
            s_CurrentlyBound = 0;
        }

        //############################################################################//
        // | UPDATE VAO STATE |
        //############################################################################//

        void set_index_buffer(const void* data, size_t count) {
            m_State->index_buffer.set_data_dynamic<unsigned int>(data, count);
        }

        void add_buffer(SimpleBuffer buffer) {
            m_State->buffers.emplace_back(buffer);
        }

        SimpleBuffer& get_buffer(size_t index) {
            ASSERT(index >= 0 && m_State->buffers.size() > index, "Index out of bounds.");
            return m_State->buffers[index];
        }

        void add_attribute(std::unique_ptr<AbstractAttributePointer>&& attribute) {
            m_State->attributes.emplace_back(std::move(attribute));
        }

        AbstractAttributePointer& get_attribute(size_t index) {
            ASSERT(index >= 0 && m_State->buffers.size() > index, "Index out of bounds.");
            return *m_State->attributes[index];
        }
    };

}

#endif //MAZEVISUALISATION_VERTEXOBJECTBINDING_H
