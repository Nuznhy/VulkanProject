#pragma once
#include "lvk_frame_info.hpp"
#include "lvk_camera.hpp"
#include "lvk_pipeline.hpp"
#include "lvk_device.hpp"
#include "lvk_model.hpp"
#include "lvk_game_object.hpp"
//std
#include <memory>
#include <vector>
namespace lvk {
    class SimpleRenderSystem {
    public:

        SimpleRenderSystem(LvkDevice  &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
        void renderGameObjects(FrameInfo &frameInfo, std::vector<LvkGameObject> &gameObjects);
    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        LvkDevice &lvkDevice;

        std::unique_ptr<LvkPipeline> lvkPipeline;
        VkPipelineLayout pipelineLayout;
    };
}