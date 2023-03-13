//
// Created by -Ry on 03/02/2023.
//

#ifndef MAZEVISUALISATION_VERTEXOBJECTBINDING_H
#define MAZEVISUALISATION_VERTEXOBJECTBINDING_H

#include "Renderer/GLUtil.h"

#include <vector>
#include <array>
#include <type_traits>
#include <glm/glm.hpp>
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
        std::string to_string() const {
            return std::format(
                    "{}", m_BufferState->to_string()
            );
        }

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
                    "[SET_DATA]", " # {:#06x}, {}, {:p}, {:#06x}",
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

        //############################################################################//
        // | SET BUFFER RANGE |
        //############################################################################//

        template<class T, auto TSize = sizeof(T)>
        void set_range(size_t offset_index, const GLvoid* data, size_t count) {
            ASSERT(is_bound(), "Can't set buffer data if the buffer is not bound...");
            GL(glBufferSubData(
                    m_BufferState->get_buffer_type(),
                    TSize * offset_index,
                    TSize * count, data
            ));
        }
    };

    //
    // Static Generator Functions.
    //

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

    template<class T,
            BufferContainerType Type,
            BufferAllocUsage Usage
    >
    static SimpleBuffer init_buffer(const T* data, const size_t size) {
        SimpleBuffer b{ Type };
        b.init();
        b.bind();
        b.set_data<T, Usage>(data, size);
        b.unbind();
        return b;
    }

    template<class T, BufferAllocUsage Usage = BufferAllocUsage::STATIC_DRAW>
    static SimpleBuffer init_array_buffer(const T* data, const size_t size) {
        return init_buffer<T, BufferContainerType::ARRAY, Usage>(data, size);
    }

    template<class T, BufferAllocUsage Usage = BufferAllocUsage::STATIC_DRAW>
    static SimpleBuffer init_uniform_buffer(const T* data, const size_t size) {
        return init_buffer<T, BufferContainerType::UNIFORM_BUFFER, Usage>(data, size);
    }

    template<class T, BufferAllocUsage Usage = BufferAllocUsage::STATIC_DRAW>
    static SimpleBuffer init_texture_buffer(const T* data, const size_t size) {
        return init_buffer<T, BufferContainerType::TEXTURE_BUFFER, Usage>(data, size);
    }

    //############################################################################//
    // | VERTEX ATTRIBUTE |
    //############################################################################//

    struct AbstractAttributePointer {

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
                GLint normalise = GL_FALSE,
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
    using UnaryIntAttribute = ComponentRestrainedAttribute<1, GLint, PrimitiveType::INT>;
    using IntVec2Attribute = ComponentRestrainedAttribute<2, GLint, PrimitiveType::INT>;
    using IntVec3Attribute = ComponentRestrainedAttribute<3, GLint, PrimitiveType::INT>;
    using UnaryUIntAttribute = ComponentRestrainedAttribute<1, GLuint, PrimitiveType::UINT>;
    using UIntVec2Attribute = ComponentRestrainedAttribute<2, GLuint, PrimitiveType::UINT>;
    using UIntVec3Attribute = ComponentRestrainedAttribute<3, GLuint, PrimitiveType::UINT>;
    using UnaryByteAttribute = ComponentRestrainedAttribute<1, GLbyte, PrimitiveType::BYTE>;
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
                GLint normalise = GL_FALSE
        ) : AbstractAttributePointer(index),
            divisor(divisor),
            normalise(normalise == GL_TRUE) {
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
    using FloatMat3Attrib = MatrixAttributePointer<3, 3, GLfloat, PrimitiveType::FLOAT>;

    //############################################################################//
    // | VERTEX ATTRIBUTE LAYOUT |
    //############################################################################//

    template<class T>
    size_t static constexpr get_gl_size() {

        if constexpr (std::is_same<glm::vec1, T>()) {
            return 1;
        } else if constexpr (std::is_same<glm::vec2, T>()) {
            return 2;
        } else if constexpr (std::is_same<glm::vec3, T>()) {
            return 3;
        } else if constexpr (std::is_same<glm::vec4, T>()) {
            return 4;
        } else {
            static_assert(always_false<T>(), "Unknown Component count for T...");
        }
    }

    template<
            class T,
            PrimitiveType GLType,
            class... Types
    >
    struct VertexLayout : AbstractAttributePointer {
        GLboolean normalise;
        GLint     divisor;

        VertexLayout(
                const GLuint index,
                GLint divisor = 0,
                GLint normalise = GL_FALSE
        ) : AbstractAttributePointer(index),
            divisor(divisor),
            normalise(normalise) {};

        template<
                typename Head,
                typename... Others
        >
        constexpr void create_attrib(
                GLuint& v_index,
                size_t stride,
                size_t& total_size
        ) {
            size_t size = get_gl_size<Head>();

            HINFO("[VERTEX_LAYOUT]", " # {}, {}, {:#06x}, {}, {}, {:p}",
                  v_index,
                  size,
                  static_cast<GLenum>(GLType),
                  normalise == GL_TRUE,
                  stride,
                  (void*) (total_size * sizeof(T))
            );

            GL(glEnableVertexAttribArray(v_index));
            GL(glVertexAttribDivisor(v_index, divisor));
            GL(glVertexAttribPointer(
                    v_index,
                    size,
                    static_cast<GLenum>(GLType),
                    normalise,
                    stride,
                    (void*) (total_size * sizeof(T))
            ));

            v_index++;
            total_size += size;
        }

        virtual void create() override {
            GLuint v_index = index;
            size_t offset  = 0;
            (create_attrib<Types>(v_index, (sizeof(Types) + ...), offset), ...);
        }

        virtual void enable() override {
            for (size_t i = 0; i < sizeof...(Types); ++i) {
                GL(glEnableVertexAttribArray(index + i));
                GL(glVertexAttribDivisor(index + i, divisor));
                ++i;
            }
        }

        virtual void disable() override {
            for (size_t i = 0; i < sizeof...(Types); ++i) {
                GL(glDisableVertexAttribArray(index + i));
                GL(glVertexAttribDivisor(index + i, 0));
                ++i;
            }
        }

    };

    using FloatAttribLayout333 = VertexLayout<float, PrimitiveType::FLOAT, glm::vec3, glm::vec3, glm::vec3>;
    using FloatAttribLayout332 = VertexLayout<float, PrimitiveType::FLOAT, glm::vec3, glm::vec3, glm::vec2>;
    using FloatAttribLayout33 = VertexLayout<float, PrimitiveType::FLOAT, glm::vec3, glm::vec3>;

    //############################################################################//
    // | BUFFER & ATTRIBUTE TOGGLES |
    //############################################################################//

    struct VaoGroup {
        std::vector<GLuint> indices;

        auto begin() {
            return indices.begin();
        }

        auto end() {
            return indices.end();
        }

        auto begin() const {
            return indices.begin();
        }

        auto end() const {
            return indices.end();
        }
    };

    //############################################################################//
    // | VERTEX ARRAY OBJECT / VAO |
    //############################################################################//

    using AttribPtr = std::unique_ptr<AbstractAttributePointer>;
    using AttributedBuffer = std::pair<SimpleBuffer, AttribPtr>;
    using BufferVector = std::vector<AttributedBuffer>;

    struct VaoState {
        GLuint       handle = 0;
        SimpleBuffer index_buffer{ BufferContainerType::ELEMENT_ARRAY };
        BufferVector buffers{};
    };

    using VaoStateHandle = std::shared_ptr<VaoState>;

    class Vao : public DelayedInit, public Bindable {

    private:
        inline static GLuint s_CurrentlyBound = 0;

    private:
        VaoStateHandle m_State = std::make_shared<VaoState>();

    public:

        //############################################################################//
        // | ASSERTIONS |
        //############################################################################//

        template<auto Expected>
        void assert_bound() {
            if constexpr (Expected) {
                ASSERT(is_bound(), "VAO needs to be bound first...");
            } else {
                ASSERT(!is_bound(), "VAO needs to be unbound...");
            }
        }

        template<auto Expected>
        void assert_init() {
            if constexpr (Expected) {
                ASSERT(is_init(), "VAO needs to be initialised...");
            } else {
                ASSERT(!is_init(), "VAO already initialised...");
            }
        }

        //############################################################################//
        // | BINDING |
        //############################################################################//

        virtual bool is_init() override {
            return m_State->handle != 0;
        }

        virtual void init() override {
            assert_init<false>();
            GL(glGenVertexArrays(1, &m_State->handle));
            m_State->index_buffer.init();
            HINFO("[VAO_INIT]", " # VAO: {:#08x}", m_State->handle);
        }

        virtual void bind() override {
            assert_init<true>();
            GL(glBindVertexArray(m_State->handle));
            s_CurrentlyBound = m_State->handle;
            m_State->index_buffer.bind();
        }

        virtual void bind_all() {
            bind();

            for (auto& [buffer, attrib] : m_State->buffers) {
                buffer.bind();
                attrib->enable();
            }
        }

        void bind(VaoGroup& targets) {
            bind();
            for (GLuint i : targets) {
                for (auto& [buffer, attrib] : m_State->buffers) {
                    if (attrib->index == i) {
                        buffer.bind();
                        attrib->enable();
                        break;
                    }
                }
            }
        }

        virtual bool is_bound() override {
            return s_CurrentlyBound == m_State->handle;
        }

        virtual void unbind() override {
            assert_init<true>();
            for (auto& [buffer, attrib] : m_State->buffers) {
                buffer.unbind();
                attrib->disable();
            }
            GL(glBindVertexArray(0));
            s_CurrentlyBound = 0;
        }

        //############################################################################//
        // | UPDATE INDEX BUFFER |
        //############################################################################//

        template<BufferAllocUsage Usage = BufferAllocUsage::DYNAMIC_DRAW>
        void set_index_buffer(const unsigned int* data, size_t count) {
            if constexpr (Usage == BufferAllocUsage::STATIC_DRAW) {
                m_State->index_buffer.set_data_static<unsigned int>(data, count);

            } else if constexpr (Usage == BufferAllocUsage::DYNAMIC_DRAW) {
                m_State->index_buffer.set_data_dynamic<unsigned int>(data, count);

            } else if constexpr (Usage == BufferAllocUsage::STREAM_DRAW) {
                m_State->index_buffer.set_data_stream<unsigned int>(data, count);

            } else {
                static_assert(always_false<decltype(Usage)>, "Unknown Usage Type...");
            }
        }

        //############################################################################//
        // | UPDATING VERTEX BUFFERS |
        //############################################################################//

        template<class T, class... Args>
        void add_buffer(SimpleBuffer buffer, Args&& ... args) {
            static_assert(
                    std::is_base_of<AbstractAttributePointer, T>(),
                    "Template must inherit from AbstractAttributePointer..."
            );
            assert_init<true>();
            assert_bound<true>();
            HINFO("[VAO_ADD_BUFFER]", " # Adding Buffer: {}", buffer.to_string());

            if (!buffer.is_bound()) buffer.bind();
            std::unique_ptr<AbstractAttributePointer> new_attrib = std::make_unique<T>(
                    T{ std::forward<Args>(args)... }
            );
            new_attrib->create();
            buffer.unbind();

            m_State->buffers.emplace_back(buffer, std::move(new_attrib));
        }

        AttributedBuffer& get_buffer(size_t slot) {

            for (auto& entry : m_State->buffers) {
                if (entry.second->index == slot) {
                    return entry;
                }
            }

            HERR("[IVO_GET]", " # Buffer with Slot/Index of value {} doesn't exist...", slot);
            throw std::exception();
        }

        template<class T, BufferAllocUsage Usage = BufferAllocUsage::STATIC_DRAW>
        void set_buffer_data(const T* data, size_t count, GLuint slot) {
            auto& [buffer, attrib] = get_buffer(slot);
            if (!buffer.is_init()) buffer.init();
            buffer.bind();
            buffer.set_data<T, Usage>(data, count);
            buffer.unbind();
        }

    };

    //############################################################################//
    // | MODEL |
    //############################################################################//

    using MeshIdentity = unsigned long long;
    using Index = unsigned int;

    // Vertex, Texture, Normal
    using IndicesVector = std::vector<glm::ivec3>;
    using Vec3Vector = std::vector<glm::vec3>;

    class Mutable3DModel {

    private:
        inline static MeshIdentity s_MeshCount = 0UL;

    private:
        long long     m_Id;
        Vec3Vector    m_Vertices{};
        Vec3Vector    m_Normals{};
        Vec3Vector    m_TextureCoords{};
        IndicesVector m_Indices{};

    public:

        Mutable3DModel() : m_Id(s_MeshCount++) {}
        Mutable3DModel(const Mutable3DModel&) = delete;

        Mutable3DModel(
                Mutable3DModel&& o
        ) : m_Id(o.m_Id),
            m_Vertices(std::move(o.m_Vertices)),
            m_Normals(std::move(o.m_Normals)),
            m_TextureCoords(std::move(o.m_TextureCoords)),
            m_Indices(std::move(o.m_Indices)) {

        };

        ~Mutable3DModel() {
            --s_MeshCount;
            HINFO("[MODEL_DELETE]", " # Deleting Model: {}", to_string());
        }

        Mutable3DModel& operator =(Mutable3DModel&& o) {
            m_Id            = o.m_Id;
            m_Vertices      = std::move(o.m_Vertices);
            m_Normals       = std::move(o.m_Normals);
            m_TextureCoords = std::move(o.m_TextureCoords);
            m_Indices       = std::move(o.m_Indices);
            return *this;
        }

        //############################################################################//
        // | GET & SET DATA |
        //############################################################################//

    public:
        const Vec3Vector& get_vertices() const {
            return m_Vertices;
        }

        size_t get_vertex_count() const {
            return m_Vertices.size();
        }

        const glm::vec3& get_vertex(size_t index) const {
            ASSERT(index < m_Vertices.size(), "Index {}..{} out of bounds...", index,
                   m_Vertices.size());
            return m_Vertices.at(index);
        }

        const glm::vec3* get_vertices_data() const {
            return m_Vertices.data();
        }

        const Vec3Vector& get_normals() const {
            return m_Normals;
        }

        size_t get_normals_count() const {
            return m_Normals.size();
        }

        const glm::vec3& get_normal(size_t index) const {
            ASSERT(index < m_Normals.size(), "Index {}..{} out of bounds...", index,
                   m_Normals.size());
            return m_Normals.at(index);
        }

        const glm::vec3* get_normals_data() const {
            return m_Normals.data();
        }

        const Vec3Vector& get_texture_coords() const {
            return m_TextureCoords;
        }

        size_t get_tex_coords_count() const {
            return m_TextureCoords.size();
        }

        const glm::vec3& get_tex_coord(size_t index) const {
            ASSERT(index < m_TextureCoords.size(), "Index {}..{} out of bounds...", index,
                   m_TextureCoords.size());
            return m_TextureCoords.at(index);
        }

        const glm::vec3* get_texture_coords_data() const {
            return m_TextureCoords.data();
        }

        void add_vert(float x, float y, float z) {
            m_Vertices.emplace_back(x, y, z);
        }

        void add_normal(float x, float y, float z) {
            m_Normals.emplace_back(x, y, z);
        }

        void add_tex_pos(float x, float y, float z) {
            m_TextureCoords.emplace_back(x, y, z);
        }

        void add_vertex_index(Index i, Index j, Index k) {
            m_Indices.emplace_back(i, j, k);
        }

        size_t get_index_count() const {
            return m_Indices.size();
        }

        //############################################################################//
        // | ITERATING DATA |
        //############################################################################//

    public:

        template<class Function>
        void for_each(Function fn) {
            for (const auto& i : m_Indices) {
                fn(get_vertex(i.x), get_normal(i.y), get_tex_coord(i.z));
            }
        }

        template<class Function>
        void for_each_vertex(Function fn) {
            for (const auto& i : m_Indices) fn(m_Vertices[i.x]);
        }

        template<class Function>
        void for_each_tri(Function fn) {
            ASSERT(
                    m_Indices.size() % 3 == 0 && m_Vertices.size() % 3 == 0,
                    "Number of vertices or indices is not a multiple of 3..."
            );
            for (Index i = 0; i < m_Indices.size();) {
                const auto& j = m_Vertices[i], k = m_Vertices[++i], l = m_Vertices[++i];
                fn(j, k, l);
            }
        }

        template<class Function>
        void for_each_index(Function fn) {
            for (const auto& index : m_Indices) {

                if constexpr (std::is_invocable<Function, Index>()) {
                    fn(index.x);

                } else if constexpr (std::is_invocable<Function, Index, Index>()) {
                    fn(index.x, index.y);

                } else if constexpr (std::is_invocable<Function, Index, Index, Index>()) {
                    fn(index.x, index.y, index.z);

                } else {
                    static_assert(
                            always_false<Function>(),
                            "Function template must have the signature"
                            " (int) or (int, int) or (int, int, int)"
                    );
                }
            }
        }

        //############################################################################//
        // | UTILITY |
        //############################################################################//

    public:
        std::string to_string() const {
            return std::format(
                    "( ID={}, Vertices={}, Normals={}, Tex-Coords={}, Indices={} )",
                    m_Id,
                    m_Vertices.size(),
                    m_TextureCoords.size(),
                    m_Normals.size(),
                    m_Indices.size()
            );
        }

        void clear() {
            m_Vertices.clear();
            m_Normals.clear();
            m_TextureCoords.clear();
            m_Indices.clear();
        }

        std::vector<Index> get_flat_indices() {
            std::vector<Index> indices{};
            for_each_index([&indices](Index i) {
                indices.push_back(i);
            });
            return indices;
        }

        std::vector<glm::vec3> flatten_vertex_data() {
            // Note: this collapses the vertex data entirely, that is,
            // 0...size is all the vertex data, and it requires no indexing.
            std::vector<glm::vec3> vertices{};
            for_each([&vertices](
                    const glm::vec3& vertex,
                    const glm::vec3& normal,
                    const glm::vec3& texture
            ) {
                vertices.insert(vertices.end(), { vertex, normal, texture });
            });
            return vertices;
        }
    };

}

#endif //MAZEVISUALISATION_VERTEXOBJECTBINDING_H
