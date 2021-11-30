#pragma once

#include "lvk_window.hpp"
#include "lvk_pipeline.hpp"
#include "lvk_device.hpp"
#include "lvk_swap_chain.hpp"
#include "lvk_model.hpp"
//std
#include <memory>
#include <vector>
namespace lvk {
    class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;
        void run();
    private:
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);

        LvkWindow lvkWindow{WIDTH, HEIGHT, "First app"};
        LvkDevice lvkDevice{lvkWindow};
        std::unique_ptr<LvkSwapChain> lvkSwapChain;
        std::unique_ptr<LvkPipeline> lvkPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<LvkModel> lvkModel;
    };
}