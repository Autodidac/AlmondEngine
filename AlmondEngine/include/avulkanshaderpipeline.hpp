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
 // avulkanshaderpipeline.hpp
#pragma once

#include "aengineconfig.hpp"
#include "avulkanquad.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <array>

#if defined(ALMOND_USING_VULKAN)
#if defined(ALMOND_USING_GLM)

namespace almondnamespace::vulkan {

    inline std::vector<char> read_shader_file(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader file: " + filepath);
        }

        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);
        file.close();

        // SPIR-V validation
        if (buffer.size() < 20 || *reinterpret_cast<uint32_t*>(buffer.data()) != 0x07230203) {
            throw std::runtime_error("Invalid SPIR-V file: " + filepath);
        }

        return buffer;
    }

    inline VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& code) {
        VkShaderModuleCreateInfo create_info{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(code.data())
        };

        VkShaderModule module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &module) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }
        return module;
    }

    struct ShaderPipeline {
        VkDevice device;
        VkRenderPass render_pass;
        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

        ShaderPipeline(VkDevice dev, VkRenderPass pass)
            : device(dev), render_pass(pass) {
        }

        void create(const std::string& vert_shader_path,
            const std::string& frag_shader_path,
            const VkExtent2D& swapchain_extent,
            VkDescriptorSetLayout descriptor_set_layout) {
            // Load shaders
            auto vert_shader_code = read_shader_file(vert_shader_path);
            auto frag_shader_code = read_shader_file(frag_shader_path);

            // Create modules
            VkShaderModule vert_module = create_shader_module(device, vert_shader_code);
            VkShaderModule frag_module = create_shader_module(device, frag_shader_code);

            // Shader stages
            std::array<VkPipelineShaderStageCreateInfo, 2> stages = { {
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vert_module,
                    .pName = "main"
                },
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = frag_module,
                    .pName = "main"
                }
            } };

            // Vertex input
            auto binding_desc = Vertex::get_binding_description();
            auto attrib_descs = Vertex::get_attribute_descriptions();

            VkPipelineVertexInputStateCreateInfo vertex_input{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &binding_desc,
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(attrib_descs.size()),
                .pVertexAttributeDescriptions = attrib_descs.data()
            };

            // Pipeline states
            VkPipelineInputAssemblyStateCreateInfo input_assembly{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE
            };

            VkViewport viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(swapchain_extent.width),
                .height = static_cast<float>(swapchain_extent.height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f
            };

            VkRect2D scissor{ .offset = {0, 0}, .extent = swapchain_extent };

            VkPipelineViewportStateCreateInfo viewport_state{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .pViewports = &viewport,
                .scissorCount = 1,
                .pScissors = &scissor
            };

            VkPipelineRasterizationStateCreateInfo rasterizer{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_BACK_BIT,
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .lineWidth = 1.0f
            };

            VkPipelineMultisampleStateCreateInfo multisampling{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .sampleShadingEnable = VK_FALSE
            };

            VkPipelineDepthStencilStateCreateInfo depth_stencil{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = VK_FALSE,
                .depthWriteEnable = VK_FALSE
            };

            VkPipelineColorBlendAttachmentState color_blend_attachment{
                .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
            };

            VkPipelineColorBlendStateCreateInfo color_blending{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = VK_FALSE,
                .attachmentCount = 1,
                .pAttachments = &color_blend_attachment
            };

            // Push constants
            VkPushConstantRange push_constant_range{
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            };

            // Pipeline layout
            VkPipelineLayoutCreateInfo pipeline_layout_info{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount = 1,
                .pSetLayouts = &descriptor_set_layout,
                .pushConstantRangeCount = 1,
                .pPushConstantRanges = &push_constant_range
            };

            if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create pipeline layout!");
            }

            // Pipeline creation
            VkGraphicsPipelineCreateInfo pipeline_info{
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount = static_cast<uint32_t>(stages.size()),
                .pStages = stages.data(),
                .pVertexInputState = &vertex_input,
                .pInputAssemblyState = &input_assembly,
                .pViewportState = &viewport_state,
                .pRasterizationState = &rasterizer,
                .pMultisampleState = &multisampling,
                .pDepthStencilState = &depth_stencil,
                .pColorBlendState = &color_blending,
                .layout = pipeline_layout,
                .renderPass = render_pass,
                .subpass = 0
            };

            if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create graphics pipeline!");
            }

            // Cleanup
            vkDestroyShaderModule(device, vert_module, nullptr);
            vkDestroyShaderModule(device, frag_module, nullptr);
        }

        ~ShaderPipeline() {
            if (pipeline) vkDestroyPipeline(device, pipeline, nullptr);
            if (pipeline_layout) vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        }
    };
}
#endif
#endif