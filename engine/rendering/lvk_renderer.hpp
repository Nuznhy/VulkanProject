#pragma once

#include "lvk_window.hpp"
#include "lvk_device.hpp"
#include "lvk_swap_chain.hpp"


//std
#include <cassert>
#include <memory>
#include <vector>
namespace lvk {
    class LvkRenderer {
    public:

        LvkRenderer(LvkWindow &window, LvkDevice &device);
        ~LvkRenderer();

        LvkRenderer(const LvkRenderer &) = delete;
        LvkRenderer &operator=(const LvkRenderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return lvkSwapChain->getRenderPass(); }
        float getAspectRation() const { return lvkSwapChain->extentAspectRatio(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when freame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        LvkWindow& lvkWindow;
        LvkDevice& lvkDevice;
        std::unique_ptr<LvkSwapChain> lvkSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}