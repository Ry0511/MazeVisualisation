//
// Header File: Renderer.h
// Date       : 21/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_RENDERER_H
#define MAZEVISUALISATION_RENDERER_H

#include "Logging.h"
#include "Renderer/VertexObjectBinding.h"
#include "Renderer/Entity.h"
#include "Renderer/Shader.h"
#include "Renderer/RendererHandlers.h"

#include <glm/glm.hpp>
#include <array>

namespace app {

    //############################################################################//
    // | TARGET RENDER GROUP |
    //############################################################################//

    class RenderGroup {

    public:
        using GroupHandlerPtr = std::unique_ptr<GroupHandler>;
        using Callback = std::function<void(RenderGroup&)>;

    public:
        inline static constexpr unsigned int s_VertexLayoutIndex      = 0U;
        inline static constexpr unsigned int s_ColourLayoutIndex      = 3U;
        inline static constexpr unsigned int s_EntityModelLayoutIndex = 4U;

    private:
        Mutable3DModel               m_Model;
        std::vector<glm::vec3>       m_VertexData;
        Vao                          m_Vao;
        Shader                       m_Shader;
        std::vector<Entity>          m_Entities;
        std::stack<Entity>           m_EntitiesToAdd;
        std::vector<GroupHandlerPtr> m_GroupHandlers;
        std::optional<Callback>      m_OnBind;
        std::optional<Callback>      m_OnUnBind;

    public:
        RenderGroup(
                Mutable3DModel&& model,
                const std::string& shader_src,
                const std::string& frag_src,
                size_t entity_count = 1
        ) : m_Model(std::move(model)),
            m_VertexData(std::move(m_Model.flatten_vertex_data())),
            m_Vao(),
            m_Shader(),
            m_Entities(),
            m_EntitiesToAdd(),
            m_GroupHandlers() {

            m_Entities.reserve(entity_count);

            // Compile Shader
            m_Shader.compile_and_link(shader_src, frag_src);
            m_Shader.enable();
            m_Shader.disable();

            // Create VAO Buffers
            m_Vao.init();
            m_Vao.bind_all();

            // Vertex Buffer (Position, Normal, Texture)
            m_Vao.add_buffer<FloatAttribLayout333>(
                    init_array_buffer<glm::vec3>(m_VertexData.data(), m_VertexData.size()),
                    s_VertexLayoutIndex, 0
            );

            constexpr BufferAllocUsage dynamic_usage = BufferAllocUsage::DYNAMIC_DRAW;

            // Colour Buffer
            m_Vao.add_buffer<Vec3Attribute>(
                    init_array_buffer<glm::vec3, dynamic_usage>(nullptr, entity_count),
                    s_ColourLayoutIndex, 1
            );

            // Model Matrix Buffer
            m_Vao.add_buffer<FloatMat4Attrib>(
                    init_array_buffer<glm::mat4, dynamic_usage>(nullptr, entity_count),
                    s_EntityModelLayoutIndex, 1
            );

            m_Vao.unbind();

        }

        RenderGroup(const RenderGroup&) = delete;

        RenderGroup(RenderGroup&& o) : m_Model(std::move(o.m_Model)),
                                       m_VertexData(std::move(o.m_VertexData)),
                                       m_Vao(o.m_Vao),
                                       m_Shader(o.m_Shader),
                                       m_Entities(std::move(o.m_Entities)),
                                       m_EntitiesToAdd(std::move(o.m_EntitiesToAdd)),
                                       m_GroupHandlers(std::move(o.m_GroupHandlers)) {

        };

    public:
        void set_on_bind(Callback cb) {
            m_OnBind = std::make_optional(cb);
        }

        void set_on_unbind(Callback cb) {
            m_OnUnBind = std::make_optional(cb);
        }

        void on_bind() {
            if (m_OnBind.has_value()) {
                m_OnBind.value()(*this);
            }
        }

        void on_unbind() {
            if (m_OnUnBind.has_value()) {
                m_OnUnBind.value()(*this);
            }
        }

        //############################################################################//
        // | GETTING DATA |
        //############################################################################//

    public:

        const Mutable3DModel& get_model() const {
            return m_Model;
        }

        const std::vector<glm::vec3> get_vertex_data() const {
            return m_VertexData;
        }

        size_t get_instance_count() const {
            return m_Entities.size();
        }

        size_t get_vertex_count() const {
            // Divide 3 as each Vertex is made of a Position, Normal, and Texture Position
            return m_VertexData.size() / 3;
        }

        Shader& get_shader() {
            return m_Shader;
        }

        void queue_entity(Entity&& entity) {
            m_EntitiesToAdd.push(std::move(entity));
        }

        Entity& get_entity(size_t index) {
            return m_Entities.at(index);
        }

        template<class T, class... Args>
        void add_group_handler(Args&& ... args) {
            static_assert(std::is_base_of<GroupHandler, T>(), "T should derive GroupHandler...");
            m_GroupHandlers.push_back(
                    std::move(
                            std::make_unique<T>(std::forward<Args>(args)...)
                    )
            );
        }

        //############################################################################//
        // | REALLOCATING DATA |
        //############################################################################//

    public:

        void realloc_entities() {
            m_Vao.bind_all();
            std::vector<glm::mat4> temp_matrices{};
            std::vector<glm::vec3> temp_colours{};
            for (Entity& e : m_Entities) {
                temp_matrices.push_back(e.get_matrix());
                temp_colours.push_back(e.get_render_attributes().colour);
            }

            // Model Matrices
            m_Vao.set_buffer_data<glm::mat4, BufferAllocUsage::DYNAMIC_DRAW>(
                    temp_matrices.data(), temp_matrices.size(), s_EntityModelLayoutIndex
            );

            // Colours
            m_Vao.set_buffer_data<glm::vec3, BufferAllocUsage::DYNAMIC_DRAW>(
                    temp_colours.data(), temp_colours.size(), s_ColourLayoutIndex
            );
            m_Vao.unbind();

            HINFO(
                    "[REALLOC_ENTITIES]",
                    " # Reallocated Entity Buffer to {} (Model Matrices & Colours).",
                    temp_colours.size()
            );
        }

        void realloc_entity(Entity& entity, size_t index) {
            auto& [model_buffer, model_slot]   = m_Vao.get_buffer(s_EntityModelLayoutIndex);
            auto& [colour_buffer, colour_slot] = m_Vao.get_buffer(s_ColourLayoutIndex);

            const glm::mat4& model  = entity.get_matrix();
            const glm::vec3& colour = entity.get_render_attributes().colour;

            m_Vao.bind_all();
            model_buffer.bind();
            model_buffer.set_range<glm::mat4>(index, &model, 1);
            model_buffer.unbind();

            colour_buffer.bind();
            colour_buffer.set_range<glm::vec3>(index, &colour, 1);
            colour_buffer.unbind();

            m_Vao.unbind();
        }

        //############################################################################//
        // | BINDING |
        //############################################################################//

    public:

        void bind() {
            m_Vao.bind_all();
            m_Shader.enable();
        }

        void unbind() {
            m_Shader.disable();
            m_Vao.unbind();
        }

    public:

        void update_entities(float delta) {

            // This block is expensive and as such should not be executed often
            bool is_realloc = false;
            while (!m_EntitiesToAdd.empty()) {
                is_realloc = true;

                Entity&& e = std::move(m_EntitiesToAdd.top());
                m_Entities.push_back(std::move(e));
                m_EntitiesToAdd.pop();
            }
            if (is_realloc) realloc_entities();

            m_Vao.bind_all();
            m_Shader.enable();

            // Update Group Functors
            for (size_t i = 0; i < m_GroupHandlers.size(); ++i) {
                GroupHandlerPtr& ptr = m_GroupHandlers[i];

                if (ptr->is_enabled(*this)) {
                    if (!ptr->update(*this, m_Vao, m_Shader)) {
                        HINFO("[TODO]", " # Remove Group Handler");
                    }
                }
            }

            // Update Entities
            for (size_t i = 0; i < m_Entities.size(); ++i) {
                Entity& e = m_Entities[i];
                e.update(*this, delta);

                // Update Entity Buffer
                if (e.is_dirty()) realloc_entity(e, i);
            }

            m_Shader.disable();
            m_Vao.unbind();
        }
    };

    //############################################################################//
    // | DRAW MODE ENUMERATION |
    //############################################################################//

    enum class DrawMode : GLenum {
        POINTS    = GL_POINTS,
        LINES     = GL_LINES,
        TRIANGLES = GL_TRIANGLES,
        POLYGONS  = GL_POLYGON
    };

    //############################################################################//
    // | RENDERER CLASS |
    //############################################################################//

    class Renderer {

    private:
        std::unordered_map<std::string, RenderGroup> m_RenderGroups;

    public:
        explicit Renderer() = default;
        ~Renderer() = default;

    public:
        void clear();
        void set_clear_colour(glm::vec4 color);
        void set_viewport(int x, int y, int w, int h);

    public:

        void create_render_group(
                const std::string& group_name,
                Mutable3DModel&& model,
                const std::string& shader_src,
                const std::string& frag_src,
                size_t entity_count = 1
        ) {
            if (!m_RenderGroups.contains(group_name)) {
                m_RenderGroups.emplace(std::make_pair(
                        group_name,
                        RenderGroup{ std::move(model),
                                     shader_src,
                                     frag_src,
                                     entity_count }
                ));
            }
        }

        RenderGroup& get_group(const std::string& group_name) {
            return m_RenderGroups.at(group_name);
        }

    public:
        void draw_buffer(DrawMode mode, GLsizei first, GLsizei count);

        void draw_buffer_instanced(
                DrawMode mode,
                GLsizei first,
                GLsizei count,
                GLsizei instance_count
        );

        void draw_elements(
                DrawMode mode,
                GLsizei count,
                PrimitiveType type = PrimitiveType::UINT,
                const GLvoid* indices = (const GLvoid*) 0
        );

        void draw_elements_instanced(
                DrawMode mode,
                GLsizei count,
                GLsizei instance_count,
                PrimitiveType type = PrimitiveType::UINT,
                const GLvoid* = (const GLvoid*) 0
        );

        void render_groups() {
            for (auto& [id, group] : m_RenderGroups) {
                group.bind();
                group.on_bind();
                draw_buffer_instanced(
                        DrawMode::TRIANGLES,
                        0,
                        group.get_vertex_count(),
                        group.get_instance_count()
                );
                group.on_unbind();
                group.unbind();
            }
        }

        void update_groups(float delta) {
            for (auto& [id, group] : m_RenderGroups) {
                group.update_entities(delta);
            }
        }

        void update_and_render_groups(float delta) {
            update_groups(delta);
            render_groups();
        }

    };

} // app

#endif
