/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
 // avulkanrenderer.hpp
#pragma once

#include "aengineconfig.hpp"
//#include "atextureatlas.hpp"
//#include "atexturecache.hpp"

#include <iostream>
#include <stdexcept>
#include <functional>

#if defined(ALMOND_USING_VULKAN)
#if defined(ALMOND_USING_GLM)

#include "avulkanglfwsetup.hpp"
#include "avulkanquad.hpp"
#include "avulkanshaderpipeline.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace almondnamespace::vulkan 
{
    // Forward declarations of Vulkan helper functions
    VkImage getTextureFromAtlas(uint32_t atlasId);
    VkImageView createImageView(VkImage image);
    VkSampler createTextureSampler();
    VkDescriptorSet getDescriptorSet(VkImageView textureImageView, VkSampler textureSampler);
    void destroyImageView(VkImageView imageView);
    void destroySampler(VkSampler sampler);

    // Vulkan context (you should define this context or pass it in)
    extern VulkanContext vulkanContext;

    // Function to set shader pipeline
    inline void set_shader_pipeline(const std::string& vert_shader_path, const std::string& frag_shader_path ){//, const VkExtent2D& swapchain_extent) {
        // Create the shader pipeline
        ShaderPipeline shader_pipeline(vulkanContext.device, vulkanContext.renderPass);

        VkDescriptorSetLayout descriptorsetlayout = nullptr;
        VkExtent2D swapchain_extent;

        // Pass the swapchain extent when creating the pipeline
        shader_pipeline.create(vert_shader_path, frag_shader_path, swapchain_extent, descriptorsetlayout);
      //  shader_pipeline.create(vert_shader_path, frag_shader_path, swapchain_extent, descriptorsetlayout);
        // After the shader pipeline is created, retrieve the pipeline and layout
        VkPipeline pipeline = shader_pipeline.pipeline;
        VkPipelineLayout pipeline_layout = shader_pipeline.pipeline_layout;

        // You can use or store the pipeline and pipeline layout as needed
    }

    // Draw sprite function (takes care of Vulkan operations for texture handling and drawing)
    inline void drawSprite(uint32_t atlasId, float x, float y, float width, float height) {
        std::cout << "Drawing sprite with ID " << atlasId << " at (" << x << ", " << y << ") with size (" << width << ", " << height << ")\n";

        VkCommandBuffer vulkanCommandBuffer = {};

        // Fetch texture from atlas
        VkImage textureImage = getTextureFromAtlas(atlasId);
        if (textureImage == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to fetch texture from atlas: texture is null.");
        }

        // Create image view for the texture
        VkImageView textureImageView = createImageView(textureImage);
        if (textureImageView == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to create texture image view.");
        }

        // Create a texture sampler for the image
        VkSampler textureSampler = createTextureSampler();
        if (textureSampler == VK_NULL_HANDLE) {
            destroyImageView(textureImageView); // Clean up the image view before throwing error
            throw std::runtime_error("Failed to create texture sampler.");
        }

        // Set up the model transformation matrix (translation and scaling)
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(width, height, 1.0f));

        // Fetch the descriptor set for the texture
        VkDescriptorSet descriptorSet = getDescriptorSet(textureImageView, textureSampler);
        if (descriptorSet == VK_NULL_HANDLE) {
            destroySampler(textureSampler); // Clean up sampler before throwing error
            destroyImageView(textureImageView); // Clean up image view
            throw std::runtime_error("Failed to create descriptor set.");
        }

        // Bind the pipeline and descriptor sets
        vkCmdBindDescriptorSets(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        vkCmdBindPipeline(vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.pipeline);

        // Record the draw call: assuming a quad with 6 indices (two triangles)
        vkCmdDraw(vulkanCommandBuffer, 6, 1, 0, 0);

        // Cleanup resources after rendering
        destroyImageView(textureImageView);
        destroySampler(textureSampler);
    }

    // Helper function to get the texture from the atlas
    VkImage getTextureFromAtlas(uint32_t atlasId) {
        VkImage image = VK_NULL_HANDLE;
        // Actual Vulkan code to fetch the texture from the atlas
        // For now, we return VK_NULL_HANDLE as a placeholder
        return image;
    }

    // Helper function to create an image view for a given image
    VkImageView createImageView(VkImage image) {
        VkImageView imageView = VK_NULL_HANDLE;
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;  // Adjust the format as needed
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vulkanContext.device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
            std::cerr << "Failed to create image view!" << std::endl;
        }
        return imageView;
    }

    // Helper function to create a texture sampler
    VkSampler createTextureSampler() {
        VkSampler sampler = VK_NULL_HANDLE;
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 1.0f;

        if (vkCreateSampler(vulkanContext.device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            std::cerr << "Failed to create texture sampler!" << std::endl;
        }
        return sampler;
    }

    // Helper function to get the descriptor set for the texture
    VkDescriptorSet getDescriptorSet(VkImageView textureImageView, VkSampler textureSampler) {
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        // Vulkan code to create and configure the descriptor set
        // Descriptor set setup is critical for binding the texture
        return descriptorSet;
    }

    // Cleanup function for the image view
    void destroyImageView(VkImageView imageView) {
        if (imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(vulkanContext.device, imageView, nullptr);
        }
    }

    // Cleanup function for the texture sampler
    void destroySampler(VkSampler sampler) {
        if (sampler != VK_NULL_HANDLE) {
            vkDestroySampler(vulkanContext.device, sampler, nullptr);
        }
    }

    // Function to begin rendering by preparing the command buffer and the pipeline
    void begin_rendering(VkCommandBuffer& commandBuffer) {
        if (vulkanContext.device == VK_NULL_HANDLE || vulkanContext.graphicsQueue == 0) {
            throw std::runtime_error("Vulkan context or graphics queue not initialized.");
        }

        // Start recording a command buffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vulkanContext.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(vulkanContext.device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer.");
        }

        // Start recording commands
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer.");
        }
    }

    // Function for submitting commands for rendering
    void render(VkCommandBuffer commandBuffer, std::function<void(VkCommandBuffer)> renderCallback) {
        // Check if the Vulkan device is initialized
        if (vulkanContext.device == VK_NULL_HANDLE) {
            throw std::runtime_error("Vulkan device is not initialized.");
        }

        // Execute the callback to render (ensure it's valid)
        renderCallback(commandBuffer);

        // Prepare the submit information
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;


        VkQueue gQueue = nullptr;

        // Submit the command buffer to the Vulkan queue
        VkResult result = vkQueueSubmit(gQueue, 1, &submitInfo, VK_NULL_HANDLE);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer.");
        }

        // Wait for the queue to idle
        result = vkQueueWaitIdle(gQueue);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to wait for graphics queue to idle.");
        }
    }

    // Function to end the rendering process
    void end_rendering() {
        // Finalize rendering, e.g., swap buffers
    }


} // namespace almondnamespace::vulkan

#endif
#endif
