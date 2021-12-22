#pragma once

#include "lvk_camera.hpp"

#include <vulkan/vulkan.h>

namespace lvk {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LvkCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}