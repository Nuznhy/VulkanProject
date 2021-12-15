#include "app.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cstdlib>
#include <ctime>


namespace lvk {

    App::App(){
        loadGameObjects();
    }

    App::~App(){ }

    void App::run() {
        SimpleRenderSystem simpleRenderSystem{lvkDevice, lvkRenderer.getSwapChainRenderPass()};
        while(!lvkWindow.shouldClose()) {
            glfwPollEvents();
            if (auto commandBuffer = lvkRenderer.beginFrame()){
                lvkRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lvkRenderer.endSwapChainRenderPass(commandBuffer);
                lvkRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lvkDevice.device());
    }

    void App::loadGameObjects() {
        // srand (static_cast <unsigned> (time(0)));
        std::vector<LvkModel::Vertex> vertices {
                {{0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        };

        auto lvkModel = std::make_shared<LvkModel>(lvkDevice, vertices);

        auto triangle = LvkGameObject::createGameObject();
        triangle.model = lvkModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .1f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}