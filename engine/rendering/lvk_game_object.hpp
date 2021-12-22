#pragma once

#include "lvk_model.hpp"

//
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace lvk {

    struct TransformComponent {
        glm::vec3 translation; // position offset
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        // translate Ry * Rx * Rz * scale transform
        // Tait-Bryan angles with y(1) x(2) z(3)
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 30.f;
    };

    class LvkGameObject {
    public:
        using id_t = unsigned int;

        using Map = std::unordered_map<id_t, LvkGameObject>;

        static LvkGameObject createGameObject() {
            static id_t currentId = 0;
            return LvkGameObject{currentId++};
        }

        static LvkGameObject makePointLight(float intensity = 10.f, glm::vec3 color = glm::vec3(1.f));

        LvkGameObject(const LvkGameObject &) = delete;
        LvkGameObject &operator=(const LvkGameObject &) = delete;
        LvkGameObject(LvkGameObject &&) = default;
        LvkGameObject &operator=(LvkGameObject &&) = default;

        id_t getId() { return id; }

        std::shared_ptr<LvkModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

        std::unique_ptr<PointLightComponent> pointLight{};

    private:
        LvkGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
}