#pragma once

#include "lvk_model.hpp"

// std
#include <memory>


namespace lvk {

    struct Transform2dComponent {
        glm::vec2 translation{}; // position offset
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};
            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
            return rotMatrix * scaleMat;
        };
    };

    class LvkGameObject{
    public:
        using id_t = unsigned int;

    static LvkGameObject createGameObject(){
        static id_t currentId = 0;
        return LvkGameObject{currentId++};
    }

    id_t getId() { return id; }

    std::shared_ptr<LvkModel> model{};
    glm::vec3 color{};
    Transform2dComponent transform2d;

    LvkGameObject(const LvkGameObject &) = delete;
    LvkGameObject &operator=(const LvkGameObject &) = delete;
    LvkGameObject(LvkGameObject&&) = default;
    LvkGameObject &operator=(LvkGameObject&&) = delete;
    private:
        LvkGameObject(id_t objId) : id{objId} {};
        id_t id;
    };
}