#pragma once

#include "SmartVkIncludes.h"
#include "SmartVkInstance.h"
#include "SmartVkQueue.h"

namespace vk
{
	class SmartVkDevice
	{
		//TODO: add a dynamic dispatcher as part of the object, and have it fed automatically into the correct functions

		/*
		* for device rating functions and device features functions it is assumed that any information required is included by the creator of the function and only device specific items are passed to the function through arguments
		* this was done because of the shear number of possible items one may need to pass in to rate a device, and there is no standard method of operation on them ruling out templates
		*/
		/*
		* device features functions are for the writting of logic behind condtionaly activated features.
		*/
		typedef vk::PhysicalDeviceFeatures2 (*deviceConditionalFeaturesFunc)(vk::PhysicalDevice);
		/*
		* device rating functions 
		*/
		typedef float (*deviceRatingFunc)(vk::PhysicalDevice, std::vector<SmartVkQueue*>, float);
		typedef float (*allocateQueueFamiliesFunc)(vk::PhysicalDevice, std::vector<SmartVkQueue*>);

	public:
		vk::Device device;
		vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE]; //devices in a group if avaiable, all devices must have the same vendorId and deviceId
		uint32_t physicalDevicesCount; //acts for isDeviceGroup

		vk::DispatchLoaderDynamic dispatcher;

		SmartVkDevice();
		SmartVkDevice(std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool createDeviceGroup);
		SmartVkDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers);
		SmartVkDevice(vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE], uint32_t physicalDeviceCount, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers);
		virtual ~SmartVkDevice();

		void pickInitDevice(std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, deviceRatingFunc rateDevice, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>(), bool createDeviceGroup = true);
		void pickInitDevice(std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>(), bool createDeviceGroup = true);
		void pickInitDevice(std::vector<SmartVkQueue*> queuePtrList, deviceConditionalFeaturesFunc getFeatures, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>(), bool createDeviceGroup = true);

		//kept seperate for debugging purposes
		void pickDevice(std::vector<SmartVkQueue*> queuePtrList, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies);

		void initDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>());
		void initDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>());
		void initDevice(vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE], uint32_t physicalDeviceCount, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>());
		void initDevice(vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE], uint32_t physicalDeviceCount, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions = std::vector<const char*>(), std::vector<const char*> debugExtensions = std::vector<const char*>(), std::vector<const char*> layers = std::vector<const char*>());

		static float defaultAllocateQueueFamilies(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList);
		static float defaultRateDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, float queuesScore);

		static vk::PhysicalDeviceFeatures featuresIntersect(vk::PhysicalDevice physicalDevice, vk::PhysicalDeviceFeatures features); //features wont change so this could be hard coded (no more will be added to device features in the vulkan spec)
		static vk::PhysicalDeviceFeatures2 featuresIntersect(vk::PhysicalDevice physicalDevice, vk::PhysicalDeviceFeatures2 features); //this cannot be done with out hard coding the feature extension types
		static std::vector<const char*> extensionsIntersect(vk::PhysicalDevice physicalDevice, std::vector<const char*> extensions);

		static vk::PhysicalDeviceFeatures2 cloneFeatures2(vk::PhysicalDeviceFeatures2 features); //would require hard coding (switch)
		static vk::PhysicalDeviceProperties2 cloneProperties2(vk::PhysicalDeviceProperties2 features); //would require hard coding (switch)
		static vk::PhysicalDeviceMemoryProperties2 cloneMemoryProperties2(vk::PhysicalDeviceMemoryProperties2 properties);

		static bool hasExtensionSupport(vk::PhysicalDevice physicalDevice, std::vector<const char*> extensions);
		static bool hasFeatureSupport(vk::PhysicalDevice physicalDevice, vk::PhysicalDeviceFeatures features);
		static bool featureMemCmp(VkBool32* features, VkBool32* deviceFeatures, size_t memSize);

		void getDeviceQueue(SmartVkQueue* queue);
		static std::vector<vk::PhysicalDevice> getAvailableDevices();
		static std::vector<vk::PhysicalDeviceGroupProperties> getAvailableDeviceGroups();
		static std::vector<SmartVkDevice*> getSelfPtrList();

		static void printAvailableDevices();
		void printAvailableExtensions();
		static void printAvailableExtensions(vk::PhysicalDevice device);
		static void printDeviceExtensionFeatures(vk::PhysicalDevice device);
	private:
		static void enumeratePhysicalDevices();
		static void enumeratePhysicalDevicesGroups();

		std::vector<SmartVkQueue*> queuePtrList;
		size_t deviceIndex;

		static std::vector<vk::PhysicalDevice> availableDevices; //required to make a this or else some underlying struct or obj is destroyed when you want to get information from the physical device handle
		static std::vector<vk::PhysicalDeviceGroupProperties> availableDeviceGroups; //same as above, i dont know if having the other std::vector would support the these structs

		static std::vector<SmartVkDevice*> selfPtrList;
	};
}