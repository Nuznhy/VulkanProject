#pragma once

#include "rendering/lvk_window.hpp"
#include "rendering/lvk_device.hpp"
#include "rendering/lvk_renderer.hpp"
#include "rendering/lvk_model.hpp"
#include "rendering/lvk_game_object.hpp"
#include "rendering/lvk_descriptors.hpp"
#include "rendering/lvk_frame_info.hpp"
//std
#include <memory>
#include <vector>
namespace lvk {
    #define MAX_LIGHTS 10

    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};  // w is intensity
    };

    struct GlobalUbo {
        glm::mat4 projectionMatrix{1.f};
        glm::mat4 viewMatrix{1.f};
        glm::mat4 invViewMatrix{1.f};
        glm::vec3 ambientLightColor{0.02f};
        int numLights;
        PointLight lights[MAX_LIGHTS];
    };
    class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;
        void run();
        void update(GlobalUbo &ubo, FrameInfo &frameInfo);
    private:
        void loadGameObjects();


        LvkWindow lvkWindow{WIDTH, HEIGHT, "First app"};
        LvkDevice lvkDevice{lvkWindow};
        LvkRenderer lvkRenderer{lvkWindow, lvkDevice};

        std::unique_ptr<LvkDescriptorPool> globalPool{};
        LvkGameObject::Map gameObjects;
    };
}