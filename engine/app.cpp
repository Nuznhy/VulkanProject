#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "lvk_buffer.hpp"
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
#include <numeric>

namespace lvk {

    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

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
            uboBuffer = std::make_unique<LvkBuffer>(lvkDevice,
                                                   sizeof(GlobalUbo),
                                                   1,
                                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                                   );
            uboBuffer->map();
        }
        auto globalSetLayout  = LvkDescriptorSetLayout::Builder(lvkDevice)
                .addBinding(0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT)
                .build();
        std::vector<VkDescriptorSet> globalDescriptorSets(LvkSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LvkDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{lvkDevice,
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
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
            if (auto commandBuffer = lvkRenderer.beginFrame()){
                int frameIndex = lvkRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lvkRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                lvkRenderer.endSwapChainRenderPass(commandBuffer);
                lvkRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lvkDevice.device());
    }


    void App::loadGameObjects() {
        std::shared_ptr<LvkModel> xwingModel = LvkModel::createModelFromFile(lvkDevice, "/home/nuznhy/CLionProjects/VulkanProject/src/models/X-home-wing.obj");
        auto xwing = LvkGameObject::createGameObject();
        xwing.model = xwingModel;
        xwing.transform.translation = {50.0f, -55.f, -5.0f};
        xwing.transform.scale = glm::vec3(1.f);
        gameObjects.push_back(std::move(xwing));

        std::shared_ptr<LvkModel> ar15Model = LvkModel::createModelFromFile(lvkDevice, "/home/nuznhy/CLionProjects/VulkanProject/src/models/ar-15.obj");
        auto ar15 = LvkGameObject::createGameObject();
        ar15.model = ar15Model;
        ar15.transform.translation = {.0f, .5f, 0.5f};
        ar15.transform.scale = glm::vec3(0.1f);
        gameObjects.push_back(std::move(ar15));

        std::shared_ptr<LvkModel> e100Model = LvkModel::createModelFromFile(lvkDevice, "/home/nuznhy/CLionProjects/VulkanProject/src/models/e100_v2.obj");
        auto e100 = LvkGameObject::createGameObject();
        e100.model = e100Model;
        e100.transform.translation = {.0f, .5f, 0.5f};
        e100.transform.scale = glm::vec3(1.f);
        gameObjects.push_back(std::move(e100));
    }
}