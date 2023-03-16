#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "rendering/lvk_buffer.hpp"
#include "rendering/lvk_camera.hpp"
#include "rendering/systems/simple_render_system.hpp"
#include "rendering/systems/point_light_system.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <filesystem>
#include <array>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <numeric>
#include <iostream>

namespace lvk {

    App::App(){
        globalPool = LvkDescriptorPool::Builder(lvkDevice)
                .setMaxSets(LvkSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LvkSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
        loadGameObjects();
    }

    App::~App(){ }

    void App::run() {
        std::vector<std::unique_ptr<LvkBuffer>> uboBuffers(LvkSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto & uboBuffer : uboBuffers) {
            uboBuffer = std::make_unique<LvkBuffer>(
                    lvkDevice,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffer->map();
        }
        auto globalSetLayout  = LvkDescriptorSetLayout::Builder(lvkDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(LvkSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LvkDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{
            lvkDevice,
            lvkRenderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{
            lvkDevice,
            lvkRenderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()};
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
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -10, 10);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

            if (auto commandBuffer = lvkRenderer.beginFrame()){
                int frameIndex = lvkRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // update
                GlobalUbo ubo{};
                update(ubo, frameInfo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lvkRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.renderLights(frameInfo);
                lvkRenderer.endSwapChainRenderPass(commandBuffer);
                lvkRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lvkDevice.device());
    }

    void App::update(GlobalUbo &ubo, FrameInfo &frameInfo) {
        ubo.projectionMatrix = frameInfo.camera.getProjection();
        ubo.viewMatrix = frameInfo.camera.getView();
        ubo.invViewMatrix = frameInfo.camera.getInverseView();

        auto rotateLights = glm::rotate(glm::mat4(1.f), frameInfo.frameTime, {0.f, -1.f, 0.f});
        int lightIndex = 0;
        for (auto &kv: frameInfo.gameObjects) {
            auto &obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            // update light position
            obj.transform.translation = glm::vec3(rotateLights * glm::vec4(obj.transform.translation, 1.f));

            // copy light to ubo
            ubo.lights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0);
            ubo.lights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
            if (++lightIndex == MAX_LIGHTS) break;
        }
        ubo.numLights = lightIndex;
    }

    void App::loadGameObjects() {
        std::shared_ptr<LvkModel> floorModel = LvkModel::createModelFromFile(lvkDevice, std::filesystem::current_path().append("../src/models/floor.obj"));
        auto floor = LvkGameObject::createGameObject();
        floor.model = floorModel;
        floor.transform.translation = {0.f, .5f, .5f};
        floor.transform.scale = {3.f, 1.f, 3.f};
        gameObjects.emplace(floor.getId(), std::move(floor));

//        std::shared_ptr<LvkModel> xwingModel = LvkModel::createModelFromFile(lvkDevice, "/home/nuznhy/CLionProjects/VulkanProject/src/models/X-home-wing.obj");
//        auto xwing = LvkGameObject::createGameObject();
//        xwing.model = xwingModel;
//        xwing.transform.translation = {50.0f, -75.f, -5.0f};
//        xwing.transform.scale = glm::vec3(1.f);
//        gameObjects.emplace(xwing.getId(), std::move(xwing));
//
        std::shared_ptr<LvkModel> ar15Model = LvkModel::createModelFromFile(lvkDevice, std::filesystem::current_path().append("../src/models/ar-15.obj"));
        auto ar15 = LvkGameObject::createGameObject();
        ar15.model = ar15Model;
        ar15.transform.translation = {.0f, 0.f, 0.0f};
        ar15.transform.scale = glm::vec3(0.1f);
        gameObjects.emplace(ar15.getId(), std::move(ar15));

        std::vector<glm::vec3> colors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < colors.size(); i++) {
            auto pointLight = LvkGameObject::makePointLight(0.5f);
            pointLight.color = colors[i];
            auto rotateLight =
                    glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / colors.size(), {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4{-1.f, -1.f, -1.f, 1.f});
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }

        std::shared_ptr<LvkModel> e100Model = LvkModel::createModelFromFile(lvkDevice, std::filesystem::current_path().append("../src/models/e100_v2.obj"));
        auto e100 = LvkGameObject::createGameObject();
        e100.model = e100Model;
        e100.transform.translation = {.5f, .5f, 0.5f};
        e100.transform.scale = glm::vec3(.1f);
        std::cout << e100.getId() << std::endl;
        gameObjects.emplace(e100.getId(), std::move(e100));
    }
}