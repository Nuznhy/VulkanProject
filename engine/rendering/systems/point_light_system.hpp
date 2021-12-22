#pragma once

#include "rendering/lvk_camera.hpp"
#include "rendering/lvk_device.hpp"
#include "rendering/lvk_frame_info.hpp"
#include "rendering/lvk_game_object.hpp"
#include "rendering/lvk_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lvk {
    class PointLightSystem {
    public:
        PointLightSystem(
                LvkDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void renderLights(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        LvkDevice &lvkDevice;

        std::unique_ptr<LvkPipeline> lvkPipeline;
        VkPipelineLayout pipelineLayout;
    };
}