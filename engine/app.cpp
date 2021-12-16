#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "lvk_camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cstdlib>
#include <ctime>
#include <chrono>

namespace lvk {

    App::App(){
        loadGameObjects();
    }

    App::~App(){ }

    void App::run() {
        SimpleRenderSystem simpleRenderSystem{lvkDevice, lvkRenderer.getSwapChainRenderPass()};

        LvkCamera camera{};


        auto viewerObject = LvkGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();


        while(!lvkWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
            cameraController.moveInPlaneXZ(lvkWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = lvkRenderer.getAspectRation();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
            if (auto commandBuffer = lvkRenderer.beginFrame()){
                lvkRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                lvkRenderer.endSwapChainRenderPass(commandBuffer);
                lvkRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lvkDevice.device());
    }


    void App::loadGameObjects() {
        std::shared_ptr<LvkModel> lvkModel = LvkModel::createModelFromFile(lvkDevice, "/home/nuznhy/CLionProjects/VulkanProject/src/models/smooth_vase.obj");
        auto gameObj = LvkGameObject::createGameObject();
        gameObj.model = lvkModel;
        gameObj.transform.translation = {.0f, .0f, 2.5f};
        gameObj.transform.scale = glm::vec3(3.f);
        gameObjects.push_back(std::move(gameObj));
    }
}