#pragma once

#include "SmartVkIncludes.h"

namespace vk
{
	/* info on what is need for extensions out side of this class:
	get window extensions,
	get debug extensions (since debug also uses ext, and not just layers, should it have a seperate entrance for them in the init,
	or maybe is there a way to tell them apart, or they can just be hard coded into the class)
	*/

	/*setting up instance:
	fillout app info struct (holds names and version numbers)
	fillout debug info struct ()
	fillout instance creat info struct (holds prev 2 structs, extension count and extensions, layer count and layers)
	create instance
	*/

	/*setting up debug messeges
	fillout debug info struct ()
	*/

	/*
	Smart instance has been made static for the simplification of smart vulkan, and since there are next to no reasons to have to instances running in one app
	*/

	class SmartVkInstance
	{
	public:
		//static vk::UniqueInstance
		static vk::Instance instance;

		static vk::DynamicLoader loader; //get rid of this too? just need it for the start of the intial init, though it may be held onto by the dispatcher

		static bool enableDebug;

		SmartVkInstance();
		SmartVkInstance(std::string appName, std::string engineName, uint32_t appVersion, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool enableDebug);
		virtual ~SmartVkInstance();

		static void initInstance(std::string appName, std::string engineName, uint32_t appVersion, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool enableDebug);
		
		//including this so that if it is more effient to have a dispatch loader for each device, each device can get a new one for itself
		static vk::DispatchLoaderDynamic getNewDispatchDynam();

		static bool hasExtensionSupport(std::vector<const char*> extensions);
		static std::vector<const char*> extensionsIntersect(std::vector<const char*> extensions);
		static std::vector<vk::ExtensionProperties> getAvailableExtensions();

		static vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo();
		static bool hasLayerSupport(std::vector<const char*> layers);
		static std::vector<vk::LayerProperties> getAvailableLayers();

		static void printAvailableExtensions();
		static void printAvailableLayers();
	private:
		static vk::DebugUtilsMessengerEXT debugUtilsMessenger;

		static void initDebugMessenger();
		static void destroyDebugMessenger();
		//static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageType, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};
}

