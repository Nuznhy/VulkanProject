#include "lvk_renderer.hpp"

#include <stdexcept>
#include <array>
#include <cstdlib>
#include <ctime>


namespace lvk {

    LvkRenderer::LvkRenderer(LvkWindow &window, LvkDevice &device)
        : lvkWindow{window}, lvkDevice{device} {
        recreateSwapChain();
        createCommandBuffers();
    }

    LvkRenderer::~LvkRenderer(){ freeCommandBuffers(); }

    void LvkRenderer::recreateSwapChain() {
        auto extent = lvkWindow.getExtent();
        while (extent.width == 0 || extent.height == 0){
            extent = lvkWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(lvkDevice.device());

        if (lvkSwapChain == nullptr) {
            lvkSwapChain = std::make_unique<LvkSwapChain>(lvkDevice, extent);
        } else {
            std::shared_ptr<LvkSwapChain> oldSwapChain = std::move(lvkSwapChain);
            lvkSwapChain = std::make_unique<LvkSwapChain>(lvkDevice, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*lvkSwapChain.get())) {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void LvkRenderer::createCommandBuffers() {
        commandBuffers.resize(LvkSwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lvkDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lvkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void LvkRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(
                lvkDevice.device(),
                lvkDevice.getCommandPool(),
                static_cast<uint32_t>(commandBuffers.size()),
                commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer LvkRenderer::beginFrame() {
        assert(!isFrameStarted && "Can't beginFrame while already in progress");
        auto result = lvkSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR){
            recreateSwapChain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire swap chain image");
        }

        isFrameStarted = true;
        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }
        return commandBuffer;
    }

    void LvkRenderer::endFrame() {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
        auto result = lvkSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lvkWindow.wasWindowResized()){
            lvkWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }

        //if (result != VK_SUCCESS){
        //    throw std::runtime_error("failed to acquire swap chain image");
        //}
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % LvkSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void LvkRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPathInfo{};
        renderPathInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPathInfo.renderPass = lvkSwapChain->getRenderPass();
        renderPathInfo.framebuffer = lvkSwapChain->getFrameBuffer(currentImageIndex);

        renderPathInfo.renderArea.offset = {0, 0};
        renderPathInfo.renderArea.extent = lvkSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPathInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPathInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPathInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lvkSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lvkSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lvkSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void LvkRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&
               "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }
}