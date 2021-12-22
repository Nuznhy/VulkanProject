#pragma once

#include "lvk_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lvk {
    class LvkDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(LvkDevice &lvkDevice) : lvkDevice{lvkDevice} {}

            Builder &addBinding(
                    uint32_t binding,
                    VkDescriptorType descriptorType,
                    VkShaderStageFlags stageFlags,
                    uint32_t count = 1);
            std::unique_ptr<LvkDescriptorSetLayout> build() const;

        private:
            LvkDevice &lvkDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        LvkDescriptorSetLayout(LvkDevice &lvkDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LvkDescriptorSetLayout();
        LvkDescriptorSetLayout(const LvkDescriptorSetLayout &) = delete;

        LvkDescriptorSetLayout &operator=(const LvkDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        LvkDevice &lvkDevice;
        VkDescriptorSetLayout descriptorSetLayout{};
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LvkDescriptorWriter;
    };
    class LvkDescriptorPool {
    public:
        class Builder {
        public:
            Builder(LvkDevice &lvkDevice) : lvkDevice{lvkDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<LvkDescriptorPool> build() const;

        private:
            LvkDevice &lvkDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        LvkDescriptorPool(
                LvkDevice &lvkDevice,
                uint32_t maxSets,
                VkDescriptorPoolCreateFlags poolFlags,
                const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~LvkDescriptorPool();
        LvkDescriptorPool(const LvkDescriptorPool &) = delete;
        LvkDescriptorPool &operator=(const LvkDescriptorPool &) = delete;

        bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        LvkDevice &lvkDevice;
        VkDescriptorPool descriptorPool;

        friend class LvkDescriptorWriter;
    };
    class LvkDescriptorWriter {
    public:
        LvkDescriptorWriter(LvkDescriptorSetLayout &setLayout, LvkDescriptorPool &pool);

        LvkDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        LvkDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        LvkDescriptorSetLayout &setLayout;
        LvkDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}