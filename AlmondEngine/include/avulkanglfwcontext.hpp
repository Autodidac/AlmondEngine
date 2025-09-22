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

#include "aengineconfig.hpp"  // #include <GLFW/glfw3.h>
#include "acontext.hpp"
#include "acontextwindow.hpp"

#ifdef ALMOND_USING_GLFW

#ifdef ALMOND_USING_VULKAN

#include "aglfwcontext.h"

#include "avulkanglfwsetup.h"

#include <vector>
#include <iostream>
#include <stdexcept>
#include <set>

#ifdef _WIN32
#include "aframework.h"
#endif

#define VK_CHECK(x)                                                                    \
	do                                                                                 \
	{                                                                                  \
		VkResult err = x;                                                              \
		if (err)                                                                       \
		{                                                                              \
			throw std::runtime_error("Detected Vulkan error: " + std::to_string(err)); \
		}                                                                              \
	} while (0)

#define ASSERT_VK_HANDLE(handle)        \
	do                                  \
	{                                   \
		if ((handle) == VK_NULL_HANDLE) \
		{                               \
			LOGE("Handle is NULL");     \
			abort();                    \
		}                               \
	} while (0)

namespace almondnamespace::vulkan {

    // External Vulkan context and GLFW window for the application
    extern VulkanContext vulkanContext = {}; // Vulkan context to hold Vulkan-related data
    extern GLFWwindow* glfwWindow = nullptr; // GLFW window handle

    // Vulkan-related resources for managing swap chain and image views
    VkQueue presentQueue = VK_NULL_HANDLE;        // Queue for presenting images to the screen
    VkQueue graphicsQueue = VK_NULL_HANDLE;       // Queue for graphics commands
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;     // Vulkan swap chain handle for image presentation
    uint32_t graphicsQueueFamily = UINT32_MAX;    // Graphics queue family index for Vulkan device
    uint32_t presentQueueFamily = UINT32_MAX;     // Presentation queue family index for Vulkan device

    std::vector<VkImage> swapChainImages;        // List of images in the swap chain
    std::vector<VkImageView> swapChainImageViews; // List of image views for the swap chain
    VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED; // Format of swap chain images (e.g., VK_FORMAT_B8G8R8A8_UNORM)
    uint32_t swapChainImagesCount = 0;           // Count of images in the swap chain
    VkExtent2D swapChainExtent = {};            // Swap chain dimensions (width & height)

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VkRenderPass renderPass;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    // Framebuffers and command buffers for rendering
    std::vector<VkFramebuffer> swapChainFramebuffers; // List of framebuffers corresponding to each swap chain image
    std::vector<VkCommandBuffer> commandBuffers;     // Command buffers for rendering operations

    //void initializeGLFW() {
    //    if (!glfwInit()) {
    //        throw std::runtime_error("Failed to initialize GLFW.");
    //    }

    //    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Disable OpenGL context as we're using Vulkan

    //    if (!glfwWindow) {
    //        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Hide the window initially
    //        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // Remove decorations if needed

    //        glfwWindow = glfwCreateWindow(800, 600, "Almond Shell - Vulkan Window", nullptr, nullptr);
    //        if (!glfwWindow) {
    //            glfwTerminate();
    //            throw std::runtime_error("Failed to create GLFW window.");
    //        }

    //        // Set the Initialized value in almond
    //        almondnamespace::contextwindow::WindowContext windowcontext;
    //        windowcontext.setWindowHandle(glfwGetWin32Window(glfwWindow));
    //    }
    //    glfwMakeContextCurrent(glfwWindow);
    //}

    void initializeGLFW() {
        glfwWindow = glfw::GLFWWindowContext::create(800, 600, "Almond Shell - Vulkan Window");
        if (!glfwWindow) {
            throw std::runtime_error("Failed to create GLFW window.");
        }
        glfwMakeContextCurrent(glfwWindow);
        //gladLoadVulkan();
    }


    //VkApplicationInfo AppInfo = {
    //.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    //.pNext = NULL,
    //.pApplicationName = "pAppName",
    //.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
    //.pEngineName = "Ogldev Vulkan Tutorials",
    //.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
    //.apiVersion = VK_API_VERSION_1_0
    //};

    //VkInstanceCreateInfo CreateInfo = {
    //    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    //    .pNext = &MessengerCreateInfo,
    //    .flags = 0,				// reserved for future use. Must be zero
    //    .pApplicationInfo = &AppInfo,
    //    .enabledLayerCount = (u32)(Layers.size()),
    //    .ppEnabledLayerNames = Layers.data(),
    //    .enabledExtensionCount = (u32)(Extensions.size()),
    //    .ppEnabledExtensionNames = Extensions.data()
    //};

    void createVulkanInstance() {
        // 1. Initialize GLFW
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // 2. Get GLFW extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // 4. Application Info
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan App";
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // 6. Instance Create Info
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();


        // Create the Vulkan instance
        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &vulkanContext.instance));

        // 8. Create Instance
 //       if (vkCreateInstance(&createInfo, nullptr, &vulkanContext.instance) != VK_SUCCESS) {
  //          throw std::runtime_error("Failed to create Vulkan instance");
 //       }
    }

    void createSurface() {
        if (vulkanContext.instance == VK_NULL_HANDLE) {
            throw std::runtime_error("createSurface(): Vulkan instance is not initialized.");
        }

        VkResult result = glfwCreateWindowSurface(vulkanContext.instance, glfwWindow, nullptr, &vulkanContext.surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan surface. Error code: " + std::to_string(result));
        }
    }

    void selectPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vulkanContext.instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vulkanContext.instance, &deviceCount, devices.data());

        vulkanContext.physicalDevice = devices[0];  // For simplicity, just take the first available device

        if (vulkanContext.physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU.");
        }

        // Get the queue family properties of the physical device
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext.physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext.physicalDevice, &queueFamilyCount, queueFamilies.data());

        int32_t graphicsQueueFamily = -1;
        int32_t presentQueueFamily = -1;

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            // Check if the queue family supports graphics
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueueFamily = i;
            }

            // Check if the queue family supports presentation (on the current surface)
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vulkanContext.physicalDevice, i, vulkanContext.surface, &presentSupport);

            if (presentSupport) {
                presentQueueFamily = i;
            }

            // If both graphics and presentation families are found, break early
            if (graphicsQueueFamily != -1 && presentQueueFamily != -1) {
                break;
            }
        }

        if (graphicsQueueFamily == -1 || presentQueueFamily == -1) {
            throw std::runtime_error("Failed to find suitable queue families.");
        }

        // Store the queue family indices in the VulkanContext
        graphicsQueueFamily = graphicsQueueFamily;
        presentQueueFamily = presentQueueFamily;
    }

    void createSwapChain() {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanContext.physicalDevice, vulkanContext.surface, &capabilities);

        VkSurfaceFormatKHR surfaceFormat;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext.physicalDevice, vulkanContext.surface, &swapChainImagesCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(swapChainImagesCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext.physicalDevice, vulkanContext.surface, &swapChainImagesCount, surfaceFormats.data());

        surfaceFormat = surfaceFormats[0];
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = capabilities.currentExtent;

        VkSwapchainCreateInfoKHR swapChainCreateInfo{};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = vulkanContext.surface;
        swapChainCreateInfo.minImageCount = capabilities.minImageCount + 1;
        swapChainCreateInfo.imageFormat = swapChainImageFormat;
        swapChainCreateInfo.imageExtent = swapChainExtent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (vkCreateSwapchainKHR(vulkanContext.device, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swapchain.");
        }

        vkGetSwapchainImagesKHR(vulkanContext.device, swapChain, &swapChainImagesCount, nullptr);
        swapChainImages.resize(swapChainImagesCount);
        vkGetSwapchainImagesKHR(vulkanContext.device, swapChain, &swapChainImagesCount, swapChainImages.data());

        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vulkanContext.device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create swapchain image views.");
            }
        }
    }

    void createLogicalDevice() {
        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily }; // Correct type

        for (uint32_t queueFamilyIndex : uniqueQueueFamilies) { // Correct type
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex; // Correct usage
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(vulkanContext.physicalDevice, &createInfo, nullptr, &vulkanContext.device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }

        vkGetDeviceQueue(vulkanContext.device, graphicsQueueFamily, 0, &graphicsQueue); // Correct: &graphicsQueue
        vkGetDeviceQueue(vulkanContext.device, presentQueueFamily, 0, &presentQueue); // Correct: &presentQueue
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vulkanContext.device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views");
            }
        }
    }

    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(vulkanContext.device, &renderPassInfo, nullptr, &vulkanContext.renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass");
        }
    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(vulkanContext.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(vulkanContext.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create synchronization objects for a frame");
            }
        }
    }
/*
    void createShaderPipeline(Device& device, const char* vert, const char* frag) {
    ShaderPipeline shaderpipeline;
        // Reuse your existing shader loading code
        //vert_module_ = shaderpipeline.create_shader_module(vert);
       // frag_module_ = shaderpipeline.create_shader_module(frag);
        //create_pipeline();
    }
*/
    void cleanupSwapChain() {
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(vulkanContext.device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(vulkanContext.device, swapChain, nullptr);
    }

    // Function to initialize Vulkan context
    void initializeVulkan() {
        // Initialize GLFW
        initializeGLFW();                 
        createVulkanInstance();           // Create Vulkan instance
        createSurface();                  // Create surface for Vulkan
        // Select physical device (GPU)
        selectPhysicalDevice();           
        createLogicalDevice();            // Create logical device
        createSwapChain();                // Create swap chain
        createRenderPass();               // Create render pass
        //createShaderPipeline();
    }

    // Function to process Vulkan rendering
    void processVulkan() {
        vkDeviceWaitIdle(vulkanContext.device); // Ensure device is idle before processing

        // Vulkan rendering commands would go here, for example:
        // - Record command buffers
        // - Submit commands to the GPU
        // - Present swap chain images
    }

    // Function to destroy resources
    void destroy() {

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
     //       vkDestroySemaphore(vulkanContext.device, renderFinishedSemaphores[i], nullptr);
      //      vkDestroySemaphore(vulkanContext.device, imageAvailableSemaphores[i], nullptr);
        //    vkDestroyFence(vulkanContext.device, inFlightFences[i], nullptr);
        }
        vkDestroyRenderPass(vulkanContext.device, vulkanContext.renderPass, nullptr);        
        
        // Cleanup swap chain resources
        for (auto& imageView : swapChainImageViews) {
            vkDestroyImageView(vulkanContext.device, imageView, nullptr);
        }
        swapChainImageViews.clear();

        // Destroy Vulkan objects
        if (vulkanContext.device != VK_NULL_HANDLE) {
            vkDestroyDevice(vulkanContext.device, nullptr);
        }
        if (swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(vulkanContext.device, swapChain, nullptr);
        }
        if (vulkanContext.surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(vulkanContext.instance, vulkanContext.surface, nullptr);
        }
        if (vulkanContext.instance != VK_NULL_HANDLE) {
            vkDestroyInstance(vulkanContext.instance, nullptr);
        }

        // Cleanup GLFW window
        if (glfwWindow) {
            glfwDestroyWindow(glfwWindow);
        }
        glfwTerminate(); // Terminate GLFW
    }

   // void submit(const VulkanDrawCommand& cmd) {
        // Convert to your existing Vulkan command buffer format
      //  vkCmdBindDescriptorSets( );
       // vkCmdDrawIndexed();
 //   }

    // Function to initialize Vulkan and handle errors
    void initialize() {
        try {
            initializeVulkan(); // Consolidated initialization into one function
            std::cout << "Vulkan initialized successfully.\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Error during initialization: " << e.what() << std::endl;
            destroy(); // Ensure proper cleanup on error
        }
    }

    // Main loop to process Vulkan rendering and GLFW events
    void process() {
        //if (glfwWindow && !glfwWindowShouldClose(glfwWindow)) {
        //    glfwPollEvents();  // Poll events for GLFW window
        //    processVulkan();   // Handle Vulkan rendering
        //}
        //else {
        //    destroy(); // Clean up and exit if the window is closed
        //}
        while (!glfw::GLFWWindowContext::should_close(glfwWindow)) {
            glfw::GLFWWindowContext::poll_events(glfwWindow);
            processVulkan();   // Handle Vulkan rendering
        }
        glfw::GLFWWindowContext::destroy(glfwWindow);
    }

    GLFWwindow* getWindow() {
        return glfwWindow;
    }

    VulkanContext& getVulkanContext() {
        return vulkanContext;
    }

} // namespace almondnamespace::vulkan

#endif // ALMOND_USING_VULKAN

#endif
