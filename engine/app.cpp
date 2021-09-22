#include "app.hpp"

#include <stdexcept>
#include <array>
#include <cstdlib>
#include <ctime>
namespace lvk {
    App::App(){
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    App::~App(){
        vkDestroyPipelineLayout(lvkDevice.device(), pipelineLayout, nullptr);
    }

    void App::run() {
        while(!lvkWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }
        vkDeviceWaitIdle(lvkDevice.device());
    }

    void App::loadModels() {
        // srand (static_cast <unsigned> (time(0)));
        std::vector<LvkModel::Vertex> vertices {
                {{0.0f, -0.5f}},
                {{0.5f, 0.5f}},
                {{-0.5f, 0.5f}}
        };

        lvkModel = std::make_unique<LvkModel>(lvkDevice, vertices);
    }

    void App::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(lvkDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline info");
        }
    }

    void App::createPipeline() {
        auto pipelineConfig = LvkPipeline::defaultPipelineConfigInfo(lvkSwapChain.width(), lvkSwapChain.height());
        pipelineConfig.renderPass = lvkSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvkPipeline = std::make_unique<LvkPipeline>(
                lvkDevice,
                "../shaders/shader.vert.spv",
                "../shaders/shader.frag.spv",
                pipelineConfig);
    }

    void App::createCommandBuffers() {
        commandBuffers.resize(lvkSwapChain.imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lvkDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lvkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers");
        }
        for (int i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }
            VkRenderPassBeginInfo renderPathInfo{};
            renderPathInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPathInfo.renderPass = lvkSwapChain.getRenderPass();
            renderPathInfo.framebuffer = lvkSwapChain.getFrameBuffer(i);

            renderPathInfo.renderArea.offset = {0, 0};
            renderPathInfo.renderArea.extent = lvkSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPathInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPathInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPathInfo, VK_SUBPASS_CONTENTS_INLINE);

            lvkPipeline->bind(commandBuffers[i]);
            lvkModel->bind(commandBuffers[i]);
            lvkModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
                throw std::runtime_error("failed to record command buffer");
            }
        }
    }
    void App::drawFrame() {
        uint32_t imageIndex;
        auto result = lvkSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire swap chain image");
        }
        result = lvkSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS){
            throw std::runtime_error("failed to acquire swap chain image");
        }
    }
}