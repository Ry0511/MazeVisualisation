//
// Created by -Ry on 04/02/2023.
//

#ifndef MAZEVISUALISATION_ENTITYCOMPONENTSYSTEM_H
#define MAZEVISUALISATION_ENTITYCOMPONENTSYSTEM_H

#include "Logging.h"

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace app {

    //############################################################################//
    // | TYPE ALIAS SIMPLIFICATIONS |
    //############################################################################//

    enum class EntityClass : uint64_t {};
    using Entity = EntityClass;

    //############################################################################//
    // | ECS Wrapper Class |
    //############################################################################//

    class EntityComponentSystem {

    private:
        entt::basic_registry<Entity> m_Registry = entt::basic_registry<Entity>{};

    public:

        //############################################################################//
        // | STATE PROBING |
        //############################################################################//

        auto get_capacity() const {
            return m_Registry.capacity();
        }

        auto total_created_count() const {
            return m_Registry.size();
        }

        auto get_alive_count() const {
            return m_Registry.alive();
        }

        auto is_valid(const Entity entity) const {
            return m_Registry.valid(entity);
        }

        //############################################################################//
        // | CREATING ENTITIES |
        //############################################################################//

        Entity create_entity() {
            return m_Registry.create();
        }

        void destroy(const Entity entity) {
            m_Registry.destroy(entity);
        }

        //############################################################################//
        // | DATA PROBING & MUTATION | CREATE, UPDATE, GET, DELETE |
        //############################################################################//

        template<class T, class... Args>
        T& add_component(const Entity entity, Args&& ... args) {
            return m_Registry.emplace<T>(entity, args...);
        }

        template<class T>
        auto add(const Entity entity, T& t) {
            m_Registry.insert<T>(entity, t);
        }

        template<class T, class... Args>
        T& replace_component(const Entity entity, Args&& ... args) {
            return m_Registry.replace<T>(entity, args...);
        }

        template<class T, class... Function>
        T& update_component(const Entity entity, Function&& ... fn) {
            return m_Registry.patch<T>(entity, fn...);
        }

        template<class T, class... Extra>
        auto delete_component(const Entity entity) {
            return m_Registry.remove<T, Extra...>(entity);
        }

        template<class T>
        T get_component(const Entity entity) {
            return m_Registry.get<T>(entity);
        }

        template<class... T>
        auto get_components(const Entity entity) {
            return m_Registry.get<T...>(entity);
        }

        bool has_no_components(const Entity entity) {
            return m_Registry.orphan(entity);
        }

        template<class... T>
        bool has_all_components(const Entity entity) {
            return m_Registry.all_of<T...>(entity);
        }

        template<class... T>
        bool has_any_component(const Entity entity) {
            return m_Registry.any_of<T...>(entity);
        }

        template<class... T>
        void clear() {
            m_Registry.clear<T...>();
        }

        void clear() {
            m_Registry.clear();
        }

        //############################################################################//
        // | ITERATING |
        //############################################################################//

        template<class... Components>
        auto get_view() {
            return m_Registry.view<Components...>();
        }

        template<class... Components>
        auto get_group() {
            return m_Registry.group<Components...>();
        }
    };

    //############################################################################//
    // | BASIC COMPONENTS |
    //############################################################################//

    struct Position {
        glm::vec3 position{ 0 };
    };

    struct Rotation {
        glm::vec3 rotation{ 0 };
    };

    struct Scale {
        glm::vec3 scale{ 1 };
    };

    struct BoxCollider {
        glm::vec3 bl;
        glm::vec3 tr;

        bool collides(const BoxCollider& o) {
            return bl.x >= o.bl.x
                   && bl.y >= o.bl.y
                   && bl.z >= o.bl.z
                   && tr.x <= o.tr.x
                   && tr.y <= o.tr.y
                   && tr.z <= o.tr.z;
        }
    };

}

#endif //MAZEVISUALISATION_ENTITYCOMPONENTSYSTEM_H
