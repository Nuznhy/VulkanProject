#pragma once
#include "rendering/lvk_frame_info.hpp"
#include "rendering/lvk_camera.hpp"
#include "rendering/lvk_pipeline.hpp"
#include "rendering/lvk_device.hpp"
#include "rendering/lvk_model.hpp"
#include "rendering/lvk_game_object.hpp"
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
        void renderGameObjects(FrameInfo &frameInfo);
    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        LvkDevice &lvkDevice;

        std::unique_ptr<LvkPipeline> lvkPipeline;
        VkPipelineLayout pipelineLayout;
    };
}