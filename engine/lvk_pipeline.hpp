#pragma once

#include "lvk_device.hpp"
#include <string>
#include <vector>
namespace lvk {
    struct PipelineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    class LvkPipeline {
    public:
        LvkPipeline(LvkDevice &device,
                    const std::string& vertFilepath,
                    const std::string& fragFilepath,
                    const PipelineConfigInfo &configInfo);
        ~LvkPipeline();
        LvkPipeline(const LvkPipeline&) = delete;
        void operator=(const LvkPipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static PipelineConfigInfo defaultPipelineConfigInfo (uint32_t width, uint32_t height);
    private:
        static std::vector<char> readFile(const std::string& filepath);
        void createGraphicsPipeline(const std::string& vertFilepath,
                                    const std::string& fragFilepath,
                                    const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char> code, VkShaderModule* shaderModule);
        LvkDevice &lvkDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };

}