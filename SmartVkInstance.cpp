
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "SmartVkInstance.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

/*
* move this info elsewhere later
* order of operations for initialization
* init window
* init instance
* init surface (part of window for the moment)
* choose then init device(s)
* 
*/

namespace vk
{
	vk::Instance SmartVkInstance::instance = nullptr;

	vk::DebugUtilsMessengerEXT SmartVkInstance::debugUtilsMessenger;
	bool SmartVkInstance::enableDebug = false;

	vk::DynamicLoader SmartVkInstance::loader;

	SmartVkInstance::SmartVkInstance()
	{
	}
	SmartVkInstance::SmartVkInstance(std::string appName, std::string engineName, uint32_t appVersion, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool enableDebug)
	{
		initInstance(appName, engineName, appVersion, extensions, layers, debugExtensions, enableDebug);
	}
	SmartVkInstance::~SmartVkInstance()
	{
		if (enableDebug)
		{
			destroyDebugMessenger();
		}
		instance.destroy();
	}

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<init vulkan instance
	void SmartVkInstance::initInstance(std::string appName, std::string engineName, uint32_t appVersion,  std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool enableDebug)
	{
		//setting up the dynamic dispatcher to find the functions need to initialize the instance?
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		SmartVkFunctions::printDebugStr("Initializing instance");
		SmartVkInstance::enableDebug = enableDebug;


		vk::ApplicationInfo appInfo = vk::ApplicationInfo(appName.data(), appVersion, engineName.data(), VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);
		vk::InstanceCreateInfo instanceInfo;
		vk::DebugUtilsMessengerCreateInfoEXT debugInfo = debugMessengerInfo();
		
		SmartVkFunctions::printDebugStr(1, "app info set");
		
		//adding debug extensions to be enabled
		if (enableDebug)
		{
			extensions = SmartVkFunctions::addCharVectors(extensions, debugExtensions);

			SmartVkFunctions::printDebugStr("checking layer support");
			if (!hasLayerSupport(layers))
			{
				SmartVkFunctions::error("layers not supported");
			}

			//TODO: see if more items should be added to the pnext chain
			instanceInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data());
			instanceInfo.pNext = &debugInfo;
			SmartVkFunctions::printDebugStr("instance debug info");
		}
		else
		{
			instanceInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo, 0, nullptr, (uint32_t)extensions.size(), extensions.data());
			instanceInfo.pNext = nullptr;
		}

		//TODO: make an extensions intersect maybe?
		//checking for extension support
		if (!hasExtensionSupport(extensions))
		{
			SmartVkFunctions::error("extensions not supported");
		}
		SmartVkFunctions::printDebugStr("extension support checked");

		//creating instance
		instance = vk::createInstance(instanceInfo);
		SmartVkFunctions::printDebugStr("instance created");
		//adding functions to dispatcher specific to the instance
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);


		//showing details of instance created
		SmartVkFunctions::printDebugStr("created instance with the following extensions:");
		for (size_t i = 0; i < extensions.size(); i++)
		{
			SmartVkFunctions::printDebugStr(2, extensions.at(i));
		}
		if(enableDebug)
		{
			SmartVkFunctions::printDebugStr(1, "and validation layers:");
			for (size_t i = 0; i < layers.size(); i++)
			{
				SmartVkFunctions::printDebugStr(2, layers.at(i));
			}
		}

		if (enableDebug)
		{
			initDebugMessenger();
			SmartVkFunctions::printDebugStr(1, "set debug messenger");
		}
	}
	vk::DispatchLoaderDynamic SmartVkInstance::getNewDispatchDynam()
	{
		if (instance = nullptr)
		{
			SmartVkFunctions::error("instance not initialized before retrieving new dispatch loader");
		}
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		vk::DispatchLoaderDynamic loader;
		loader.init(vkGetInstanceProcAddr);
		loader.init(instance);

		return loader;
	}
	bool SmartVkInstance::hasExtensionSupport(std::vector<const char*> extensions)
	{
		std::vector<vk::ExtensionProperties> availableExtensions = getAvailableExtensions();

		bool found = false;
		for (size_t i = 0; i < extensions.size(); i++)
		{
			found = false;
			for (size_t j = 0; j < availableExtensions.size() && !found; j++)
			{
				if (strcmp(extensions.at(i), availableExtensions.at(j).extensionName) == 0)
				{
					found = true;
				}
			}
			if (!found)
			{
				return false;
			}
		}
		return true;
	}
	std::vector<const char*> SmartVkInstance::extensionsIntersect(std::vector<const char*> extensions)
	{
		return std::vector<const char*>();
	}
	std::vector<vk::ExtensionProperties> SmartVkInstance::getAvailableExtensions()
	{
		std::vector<vk::ExtensionProperties> extensions;
		extensions = vk::enumerateInstanceExtensionProperties(nullptr);

		return extensions;
	}


	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<debug and validation layers
	void SmartVkInstance::initDebugMessenger()
	{
		debugUtilsMessenger = instance.createDebugUtilsMessengerEXT(debugMessengerInfo(), nullptr);
	}
	//error message call back
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		//if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) 
		//{
		//}

		return VK_FALSE;
	}
	void SmartVkInstance::destroyDebugMessenger()
	{
		instance.destroyDebugUtilsMessengerEXT(debugUtilsMessenger, nullptr);
	}
	vk::DebugUtilsMessengerCreateInfoEXT SmartVkInstance::debugMessengerInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		return createInfo;
	}
	bool SmartVkInstance::hasLayerSupport(std::vector<const char*> layers)
	{
		std::vector<vk::LayerProperties> availableLayers = getAvailableLayers();

		bool found = false;
		for (size_t i = 0; i < layers.size(); i++)
		{
			found = false;
			for (size_t j = 0; j < availableLayers.size() && !found; j++)
			{
				if (strcmp(layers.at(i), availableLayers.at(j).layerName) == 0)
				{
					found = true;
				}
			}
			if (!found)
			{
				return false;
			}
		}

		return true;
	}
	std::vector<vk::LayerProperties> SmartVkInstance::getAvailableLayers()
	{
		std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

		return availableLayers;
	}

	void SmartVkInstance::printAvailableExtensions()
	{
		std::vector<vk::ExtensionProperties> extensions = getAvailableExtensions();

		for (int i = 0; i < extensions.size(); i++)
		{
			std::cout << '\t' << extensions.at(i).extensionName << '\n';
		}
	}
	void SmartVkInstance::printAvailableLayers()
	{
		std::vector<vk::LayerProperties> layers = getAvailableLayers();

		for (int i = 0; i < layers.size(); i++)
		{
			std::cout << '\t' << layers.at(i).layerName << '\n';
		}
	}
}