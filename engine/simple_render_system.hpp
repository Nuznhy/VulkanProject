#pragma once

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

        SimpleRenderSystem(LvkDevice  &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LvkGameObject> &gameObjects,
                               const LvkCamera &camera);
    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        LvkDevice &lvkDevice;

        std::unique_ptr<LvkPipeline> lvkPipeline;
        VkPipelineLayout pipelineLayout;
    };
}