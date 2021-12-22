#pragma once

#include "rendering/lvk_camera.hpp"
#include "rendering/lvk_descriptors.hpp"
#include "rendering/lvk_game_object.hpp"
#include <vulkan/vulkan.h>

namespace lvk {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LvkCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        LvkGameObject::Map &gameObjects;
    };
}