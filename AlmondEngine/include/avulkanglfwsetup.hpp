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

#pragma once

#include "aengineconfig.hpp" // <GLFW/glfw3.h> // Add GLFW header for extension retrieval

#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>
#include <optional>

#if defined(ALMOND_USING_VULKAN)

namespace almondnamespace::vulkan {

    struct VulkanContext {
        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkShaderModule vertShaderModule = VK_NULL_HANDLE;
        VkShaderModule fragShaderModule = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE; // Command buffer to store rendering commands
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    };

#if !defined(USE_VOLK)
    struct VulkanFunctions {
        PFN_vkCreateInstance vkCreateInstance = nullptr;
        PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
        PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
        PFN_vkCreateDevice vkCreateDevice = nullptr;
        PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;
        PFN_vkDestroyInstance vkDestroyInstance = nullptr;
        PFN_vkDestroyDevice vkDestroyDevice = nullptr;
        PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
        PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = nullptr;
    };

    inline VulkanFunctions loadVulkanFunctions() {
        VulkanFunctions vkFuncs;

#if defined (_WIN32)
        HMODULE vulkanLibrary = LoadLibraryW(L"vulkan-1.dll");
        if (!vulkanLibrary) {
            throw std::runtime_error("Failed to load Vulkan library.");
        }

        auto loadFunction = [&](const char* name) -> void* {
            return reinterpret_cast<void*>(GetProcAddress(vulkanLibrary, name));
            };
#else
        void* vulkanLibrary = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
        if (!vulkanLibrary) {
            throw std::runtime_error("Failed to load Vulkan library.");
        }

        auto loadFunction = [&](const char* name) -> void* {
            return dlsym(vulkanLibrary, name);
            };
#endif

        vkFuncs.vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(loadFunction("vkCreateInstance"));
        vkFuncs.vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(loadFunction("vkEnumeratePhysicalDevices"));
        vkFuncs.vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(loadFunction("vkGetPhysicalDeviceQueueFamilyProperties"));
        vkFuncs.vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(loadFunction("vkCreateDevice"));
        vkFuncs.vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(loadFunction("vkGetDeviceQueue"));
        vkFuncs.vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(loadFunction("vkDestroyInstance"));
        vkFuncs.vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(loadFunction("vkDestroyDevice"));
        vkFuncs.vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(loadFunction("vkDestroySurfaceKHR"));
        vkFuncs.vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(loadFunction("vkCreateWin32SurfaceKHR"));

        return vkFuncs;
    }
#endif

    inline VulkanContext InitializeVulkanRenderer() {
#if defined(USE_VOLK)
        if (volkInitialize() != VK_SUCCESS) {
            throw std::runtime_error("Failed to initialize Volk.");
        }
#endif

        VulkanContext context;

        // Setup Vulkan application info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Almond Shell Example";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        // Create Vulkan instance
        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        if (!glfwExtensions) {
            throw std::runtime_error("Failed to get required Vulkan extensions from GLFW.");
        }
        instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
        instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &context.instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance.");
        }

#if defined(USE_VOLK)
        volkLoadInstance(context.instance);
#endif

        // Create the command pool
        VkCommandPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.queueFamilyIndex = 0; // Replace with the actual queue family index
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Command buffers can be reset

        result = vkCreateCommandPool(context.device, &poolCreateInfo, nullptr, &context.commandPool);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan command pool.");
        }

        std::cout << "Vulkan initialized successfully." << std::endl;
        return context;
    }

    inline void createCommandBuffer(VulkanContext& context) {
        // Allocate a command buffer from the pool
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = context.commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary command buffer for graphics
        allocateInfo.commandBufferCount = 1; // Number of command buffers to allocate

        VkResult result = vkAllocateCommandBuffers(context.device, &allocateInfo, &context.commandBuffer);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer.");
        }
    }

    inline void beginCommandBufferRecording(VulkanContext& context) {
        // Start recording commands in the command buffer
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Allow simultaneous usage if needed
        beginInfo.pInheritanceInfo = nullptr; // Only needed for secondary command buffers

        VkResult result = vkBeginCommandBuffer(context.commandBuffer, &beginInfo);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer recording.");
        }
    }

    inline void endCommandBufferRecording(VulkanContext& context) {
        // End the command buffer recording
        VkResult result = vkEndCommandBuffer(context.commandBuffer);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to end command buffer recording.");
        }
    }

    inline void submitCommandBuffer(VulkanContext& context) {
        // Submit the command buffer to the graphics queue for execution
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &context.commandBuffer;

        VkQueue gQueue = nullptr;

        VkResult result = vkQueueSubmit(gQueue, 1, &submitInfo, VK_NULL_HANDLE);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit command buffer.");
        }
    }

    inline void cleanupVulkan(VulkanContext& context) {
        // Cleanup Vulkan resources
        if (context.commandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(context.device, context.commandPool, nullptr);
        }

        if (context.device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(context.device);
            vkDestroyDevice(context.device, nullptr);
        }
        if (context.instance != VK_NULL_HANDLE) {
            vkDestroyInstance(context.instance, nullptr);
        }
        std::cout << "Vulkan resources cleaned up." << std::endl;
    }

} // namespace almondnamespace::vulkan

#endif
