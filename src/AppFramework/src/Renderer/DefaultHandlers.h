//
// Created by -Ry on 12/03/2023.
//

#ifndef MAZEVISUALISATION_DEFAULTHANDLERS_H
#define MAZEVISUALISATION_DEFAULTHANDLERS_H

#include "RendererHandlers.h"
#include "Entity.h"

namespace app {

    //############################################################################//
    // | ARBITRARILY ROTATE WITH A BIAS |
    //############################################################################//

    class RotationHandler : public EntityHandler {

    private:
        float     m_Theta;
        glm::vec3 m_RotationBias;

    public:

        RotationHandler(
                const glm::vec3& rotation_bias
        ) : m_Theta(),
            m_RotationBias(rotation_bias) {

        };

        virtual bool is_enabled(Entity& entity) override {
            return true;
        }

        virtual bool update(Entity& entity, RenderGroup& group, float d) override {
            m_Theta += d;
            Transform& t = entity.get_transform();
            t.rotate = glm::vec3{
                    m_Theta * m_RotationBias.x,
                    m_Theta * m_RotationBias.y,
                    m_Theta * m_RotationBias.z
            };
            entity.set_dirty();
            return true;
        }
    };

    //############################################################################//
    // | SINGULAR LERP FROM A -> B |
    //############################################################################//

    class LerpTo : public EntityHandler {
    private:
        glm::vec3 m_Start;
        glm::vec3 m_Destination;
        float     m_Speed;
        float     m_Theta;

    public:
        LerpTo(
                const glm::vec3& start,
                const glm::vec3& dest,
                float speed = 1.0F
        ) : m_Start(start),
            m_Destination(dest),
            m_Speed(speed),
            m_Theta() {

        }

    public:

        virtual bool is_enabled(Entity& entity) override {
            return true;
        }

        virtual bool update(Entity& entity, RenderGroup& group, float delta) override {

            Transform& t = entity.get_transform();
            m_Theta += (m_Speed * delta);
            t.pos = glm::mix(m_Start, m_Destination, m_Theta);
            entity.set_dirty();
            return m_Theta < 1.0F;
        }
    };

    //############################################################################//
    // | ARBITRARILY SHRINK |
    //############################################################################//

    class ArbitraryScaleHandler : public EntityHandler {

    private:
        glm::vec3 m_Bias;
        float     m_Theta;

    public:
        ArbitraryScaleHandler(const glm::vec3& bias = { 1, 1, 1 }) : m_Bias(bias), m_Theta() {}

    public:
        virtual bool is_enabled(Entity& entity) override {
            return true;
        }

        virtual bool update(Entity& entity, RenderGroup& group, float delta) override {
            m_Theta += delta;
            Transform& t = entity.get_transform();
            t.scale = glm::vec3{
                    std::sin(m_Theta * m_Bias.x),
                    std::sin(m_Theta * m_Bias.y),
                    std::sin(m_Theta * m_Bias.z)
            };
            entity.set_dirty();
            return true;
        }
    };

    //############################################################################//
    // | COLOUR SKEW HANDLER |
    //############################################################################//

    class ColourSkew : public EntityHandler {

    public:
        inline static constexpr float s_UpdateTimeframe = 1.0F / 20.0F;

    private:
        glm::vec3 m_Bias;
        glm::vec3 m_Min;
        glm::vec3 m_Max;
        float     m_Theta = 0.0F;
        float     m_Delta = 0.0F;

    public:
        ColourSkew(
                glm::vec3 bias,
                glm::vec3 min,
                glm::vec3 max
        ) : m_Bias(bias),
            m_Min(min),
            m_Max(max),
            m_Theta() {

        };

    public:
        virtual bool is_enabled(Entity& entity) override {
            return true;
        }

        virtual bool update(Entity& entity, RenderGroup& group, float d) override {
            m_Theta += d;
            m_Delta += d;

            // Publish results only so often
            if (m_Delta > s_UpdateTimeframe) {
                entity.set_dirty();

                // Skew Colour
                entity.get_render_attributes().colour = glm::vec3{
                        std::clamp(std::sin(m_Theta) * m_Bias.r, m_Min.r, m_Max.r),
                        std::clamp(std::cos(m_Theta) * m_Bias.g, m_Min.g, m_Max.g),
                        std::clamp(std::sin(m_Theta) * m_Bias.b, m_Min.b, m_Max.b)
                };

                m_Delta = 0.0F;
            }

            return true;
        }
    };

}

#endif //MAZEVISUALISATION_DEFAULTHANDLERS_H
