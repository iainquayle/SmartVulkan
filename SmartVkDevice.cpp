#include "SmartVkDevice.h"

namespace vk
{
	std::vector<vk::PhysicalDevice> SmartVkDevice::availableDevices;
	std::vector<vk::PhysicalDeviceGroupProperties> SmartVkDevice::availableDeviceGroups;
	std::vector<SmartVkDevice*> SmartVkDevice::selfPtrList;

	SmartVkDevice::SmartVkDevice()
		:device(nullptr), 
		physicalDevices(),
		physicalDevicesCount(0),
		dispatcher(),
		queuePtrList(std::vector<SmartVkQueue*>()),
		deviceIndex(0)
	{
		selfPtrList.push_back(this);
	}
	SmartVkDevice::SmartVkDevice(std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool createDeviceGroup)
		:device(nullptr),
		physicalDevices(),
		physicalDevicesCount(0),
		dispatcher(),
		queuePtrList(std::vector<SmartVkQueue*>()),
		deviceIndex(0)
	{
		pickInitDevice(queuePtrList, features, rateDevice, allocateQueueFamilies, extensions, debugExtensions, layers, createDeviceGroup);
		selfPtrList.push_back(this);
	}
	SmartVkDevice::SmartVkDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers)
		:device(nullptr),
		physicalDevices(),
		physicalDevicesCount(0),
		dispatcher(),
		queuePtrList(std::vector<SmartVkQueue*>()),
		deviceIndex(0)
	{
		initDevice(physicalDevice, queuePtrList, features, allocateQueueFamilies, extensions, debugExtensions, layers);
		selfPtrList.push_back(this);
	}
	SmartVkDevice::SmartVkDevice(vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE], uint32_t physicalDeviceCount, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers)
		:device(nullptr),
		physicalDevices(),
		physicalDevicesCount(0),
		dispatcher(),
		queuePtrList(std::vector<SmartVkQueue*>()),
		deviceIndex(0)
	{

		initDevice(physicalDevices, physicalDeviceCount, queuePtrList, features, allocateQueueFamilies, extensions, debugExtensions, layers);
		selfPtrList.push_back(this);
	}
	SmartVkDevice::~SmartVkDevice()
	{
		vkDestroyDevice(device, nullptr);
	}

	void SmartVkDevice::pickInitDevice(std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, deviceRatingFunc rateDevice, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool createDeviceGroup)
	{
		pickInitDevice(queuePtrList, features, rateDevice, defaultAllocateQueueFamilies, extensions, debugExtensions, layers, createDeviceGroup);
	}
	void SmartVkDevice::pickInitDevice(std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool createDeviceGroup)
	{
		pickDevice(queuePtrList, rateDevice, allocateQueueFamilies);
		if (!createDeviceGroup)
		{
			physicalDevicesCount = 1;
		}
		initDevice(physicalDevices, physicalDevicesCount, queuePtrList, features, extensions, debugExtensions, layers);
	}
	void SmartVkDevice::pickInitDevice(std::vector<SmartVkQueue*> queuePtrList, deviceConditionalFeaturesFunc getFeatures, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers, bool createDeviceGroup)
	{
		pickDevice(queuePtrList, rateDevice, allocateQueueFamilies);
		if (!createDeviceGroup)
		{
			physicalDevicesCount = 1;
		}
		initDevice(physicalDevices, physicalDevicesCount, queuePtrList, getFeatures(physicalDevices[0]), extensions, debugExtensions, layers);
	}

	void SmartVkDevice::pickDevice(std::vector<SmartVkQueue*> queuePtrList, deviceRatingFunc rateDevice, allocateQueueFamiliesFunc allocateQueueFamilies)
	{
		SmartVkFunctions::printDebugStr(0, "picking physical device", true);
		enumeratePhysicalDevices();
		if (availableDevices.size() == 0)
		{
			SmartVkFunctions::error("no devices found");
		}

		vk::PhysicalDeviceProperties properties;
		float highScore = 0.0f, tempScore = 0.0f, queueScore = 0.0f;
		size_t highScoreIndex = 0;
		for (size_t i = 0; i < availableDevices.size(); i++)
		{
			properties = availableDevices.at(i).getProperties();
			std::cout << "\trating device: " << properties.deviceName << '\n';
			SmartVkFunctions::printDebugStr(1, "rating device:", false);  //maybe make print debug take in a list of strings, then just have an overloaded one with one
			SmartVkFunctions::printDebugStr((std::string)properties.deviceName);

			SmartVkFunctions::printDebugStr(2, "rating device queue allocation");
			if (allocateQueueFamilies == nullptr)
			{
				queueScore = defaultAllocateQueueFamilies(availableDevices.at(i), queuePtrList);
			}
			else
			{
				queueScore = allocateQueueFamilies(availableDevices.at(i), queuePtrList);
			}

			SmartVkFunctions::printDebugStr(2, "rating device properties");
			if (rateDevice == nullptr)
			{
				tempScore = defaultRateDevice(availableDevices.at(i), queuePtrList, queueScore);
			}
			else
			{
				tempScore = rateDevice(availableDevices.at(i), queuePtrList, queueScore);
			}

			if (tempScore > highScore)
			{
				highScore = tempScore;
				highScoreIndex = i;
			}
		}
		if (highScore <= 0.0f)
		{
			SmartVkFunctions::error("failed to find suitable device");
		}

		deviceIndex = highScoreIndex;
		physicalDevices[0] = availableDevices.at(highScoreIndex);

		properties = availableDevices.at(highScoreIndex).getProperties();
		SmartVkFunctions::printDebugStr(1, "device picked:", false);  //maybe make print debug take in a list of strings, then just have an overloaded one with one
		SmartVkFunctions::printDebugStr((std::string)properties.deviceName);
	}

	void SmartVkDevice::initDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers)
	{
		initDevice(physicalDevice, queuePtrList, features, defaultAllocateQueueFamilies, extensions, debugExtensions, layers);
	}
	void SmartVkDevice::initDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers)
	{
		physicalDevices[0] = physicalDevice;
		physicalDevicesCount = 1;
		initDevice(physicalDevices, physicalDevicesCount, queuePtrList, features, allocateQueueFamilies, extensions, debugExtensions, layers);
	}
	void SmartVkDevice::initDevice(vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE], uint32_t physicalDeviceCount, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers)
	{
		initDevice(physicalDevices, physicalDeviceCount, queuePtrList, features, defaultAllocateQueueFamilies, extensions, debugExtensions, layers);
	}
	void SmartVkDevice::initDevice(vk::PhysicalDevice physicalDevices[VK_MAX_DEVICE_GROUP_SIZE], uint32_t physicalDeviceCount, std::vector<SmartVkQueue*> queuePtrList, vk::PhysicalDeviceFeatures2 features, allocateQueueFamiliesFunc allocateQueueFamilies, std::vector<const char*> extensions, std::vector<const char*> debugExtensions, std::vector<const char*> layers)
	{
		std::cout << "initiating logical device\n";
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::vector<std::vector<float>> queuePriorities;
		bool queueFamilyFound;

		//re organizing queues
		allocateQueueFamilies(physicalDevices[0], queuePtrList);

		//creating lists of queue priorities for queue create info structs
		for (size_t i = 0; i < queuePtrList.size(); i++)
		{
			queueFamilyFound = false;
			for (size_t j = 0; j < queueCreateInfos.size() && !queueFamilyFound; j++)
			{
				if (queuePtrList.at(i)->queueFamilyIndex == queueCreateInfos.at(j).queueFamilyIndex)
				{
					queuePtrList.at(i)->indexInQueueFamily = queueCreateInfos.at(j).queueCount;
					queuePriorities.at(j).push_back(queuePtrList.at(i)->queuePriority);
					queueFamilyFound = true;
				}
			}
			if (!queueFamilyFound)
			{
				// TODO: add in ability for use of global priority (pnext) ()
				queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(0), queuePtrList.at(i)->queueFamilyIndex, 1, nullptr));
				queuePriorities.push_back(std::vector<float>(1, queuePtrList.at(i)->queuePriority));
			}
		}
		for (size_t i = 0; i < queueCreateInfos.size(); i++)
		{
			queueCreateInfos.at(i).setQueueCount((uint32_t)queuePriorities.at(i).size());
			queueCreateInfos.at(i).setPQueuePriorities(queuePriorities.at(i).data());
		}

		if (SmartVkInstance::enableDebug)
		{
			extensions = SmartVkFunctions::addCharVectors(extensions, debugExtensions);
		}
		else
		{
			layers = std::vector<const char*>(0);
		}

		vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(0), (uint32_t)queueCreateInfos.size(), queueCreateInfos.data(), (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data(), &(features.features));
		createInfo.setPNext(&features);

		vk::DeviceGroupDeviceCreateInfo groupCreateInfo(physicalDeviceCount, physicalDevices);
		groupCreateInfo.setPNext(&features);
		if (physicalDeviceCount > 1)
		{
			createInfo.setPNext(&groupCreateInfo);
		}

		device = physicalDevices[0].createDevice(createInfo);
		std::cout << "\tlogical device and queues created\n";

		//TODO: change to distribute device queues function?
		for (size_t i = 0; i < queuePtrList.size(); i++)
		{
			queuePtrList.at(i)->queue = device.getQueue(queuePtrList.at(i)->queueFamilyIndex, queuePtrList.at(i)->indexInQueueFamily);
		}
		std::cout << "\tqueues assigned to smart queues\n";

		dispatcher = SmartVkInstance::getNewDispatchDynam();
		dispatcher.init(device);
	}

	float SmartVkDevice::defaultAllocateQueueFamilies(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList)
	{
		/*
		reseting all found queue indicators to false, queueFamilyIndex doesnt need to be reset because of this
		*/
		for (size_t i = 0; i < queuePtrList.size(); i++)
		{
			queuePtrList.at(i)->foundQueue = false;
		}

		std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
		std::vector<uint32_t> queueCountAvailable(queueFamilies.size());
		/*
		variable size queue counts for each family
		*/

		for (size_t i = 0; i < queueFamilies.size(); i++)
		{
			queueCountAvailable.at(i) = queueFamilies.at(i).queueCount;
		}

		float score = 1.0f;
		bool madeChange = true;
		while (madeChange)
		{
			/*
			setting queue indices if they only have one available queue family
			*/
			madeChange = false;
			for (size_t i = 0; i < queuePtrList.size(); i++)
			{
				if (!queuePtrList.at(i)->foundQueue)
				{
					size_t queueCount = 0;
					size_t queueFamilyCount = 0;
					for (size_t j = 0; j < queueFamilies.size(); j++)
					{
						if (((queuePtrList.at(i)->flag & queueFamilies.at(j).queueFlags) == queuePtrList.at(i)->flag) && queueCountAvailable.at(j) > 0)
						{
							queueFamilyCount++;
							queueCount += queueCountAvailable.at(j);
						}
					}
					if (queueCount == 0)
					{
						return 0.0;
					}
					if (queueFamilyCount == 1)
					{
						for (size_t j = 0; j < queueFamilies.size(); j++)
						{
							if ((queuePtrList.at(i)->flag & queueFamilies.at(j).queueFlags) == queuePtrList.at(i)->flag)
							{
								queuePtrList.at(i)->queueFamilyIndex = (uint32_t)j;
								queuePtrList.at(i)->foundQueue = true;
								queueCountAvailable.at(j)--;
								madeChange = true;
								if (queueFamilies.at(j).queueFlags != queuePtrList.at(i)->flag)
								{
									score *= ((1.0f - queuePtrList.at(i)->soloQueuePriority) * 0.5f) + 0.5f;
								}
							}
						}
					}
				}
			}
			/*
			set one queue index if no queues were set previously
				the queue with the least available possible queues is set
				if the solo queue priority is higher than that of others requesting a solo queue, it is given one
				else it is given a family with the most available queues left
			*/

			//review the following to see if solo queue check is proper, cant remeber
			if (!madeChange)
			{
				size_t minQueueCount = SIZE_MAX;
				size_t minQueueCountIndex = 0;
				for (size_t i = 0; i < queuePtrList.size(); i++)//finding min
				{
					if (!queuePtrList.at(i)->foundQueue)
					{
						size_t tempCount = 0;
						for (size_t j = 0; j < queueFamilies.size(); j++)
						{
							if ((queuePtrList.at(i)->flag & queueFamilies.at(j).queueFlags) == queuePtrList.at(i)->flag)
							{
								tempCount += queueCountAvailable.at(j);
							}
						}
						if (tempCount < minQueueCount)
						{
							minQueueCount = tempCount;
							minQueueCountIndex = i;
						}
						madeChange = true;
					}
				}
				if (minQueueCount == 0)
				{
					return 0.0f;
				}
				if (madeChange)
				{
					size_t soloQueueIndex = 0;
					for (size_t i = 0; i < queuePtrList.size(); i++)
					{
						if (queuePtrList.at(minQueueCountIndex)->flag == queuePtrList.at(i)->flag && !queuePtrList.at(i)->foundQueue)
						{
							if ((queuePtrList.at(minQueueCountIndex)->soloQueuePriority < queuePtrList.at(i)->soloQueuePriority) || (queuePtrList.at(minQueueCountIndex)->soloQueuePriority == queuePtrList.at(i)->soloQueuePriority && i < minQueueCountIndex))
							{
								soloQueueIndex++;
							}
						}
					}
					for (size_t i = 0; i < queueFamilies.size() && !queuePtrList.at(minQueueCountIndex)->foundQueue; i++)
					{
						if (queueFamilies.at(i).queueFlags == queuePtrList.at(minQueueCountIndex)->flag && queueCountAvailable.at(i) >= soloQueueIndex)
						{
							queuePtrList.at(minQueueCountIndex)->queueFamilyIndex = (uint32_t)i;
							queuePtrList.at(minQueueCountIndex)->foundQueue = true;
							queueCountAvailable.at(i)--;
						}
					}
					if (!queuePtrList.at(minQueueCountIndex)->foundQueue)
					{
						size_t maxQueueCountAvailable = 0;
						size_t maxQueueCountAvailableIndex = 0;
						for (size_t i = 0; i < queueFamilies.size(); i++)
						{
							if (maxQueueCountAvailable < queueCountAvailable.at(i))
							{
								maxQueueCountAvailable = queueCountAvailable.at(i);
								maxQueueCountAvailableIndex = i;
							}
						}
						queuePtrList.at(minQueueCountIndex)->queueFamilyIndex = (uint32_t)maxQueueCountAvailableIndex;
						queuePtrList.at(minQueueCountIndex)->foundQueue = true;
						queueCountAvailable.at(maxQueueCountAvailableIndex)--;
						score *= ((1.0f - queuePtrList.at(minQueueCountIndex)->soloQueuePriority) * 0.5f) + 0.5f;
					}
				}
			}
		}
		return score;
	}
	// TODO: finish default rating func
	float SmartVkDevice::defaultRateDevice(vk::PhysicalDevice physicalDevice, std::vector<SmartVkQueue*> queuePtrList, float queuesScore)
	{
		std::cout << "default rating not yet implemented";
		return queuesScore;
	}

	vk::PhysicalDeviceFeatures SmartVkDevice::featuresIntersect(vk::PhysicalDevice physicalDevice, vk::PhysicalDeviceFeatures features)
	{
		vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();
		VkPhysicalDeviceFeatures intersection;

		for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++)
		{
			if ((*((VkBool32*)(&features) + i) && (*((VkBool32*)(&deviceFeatures) + i))))
			{
				*((VkBool32*)(&intersection) + i) = true;
			}
		}

		return intersection;
	}
	vk::PhysicalDeviceFeatures2 SmartVkDevice::featuresIntersect(vk::PhysicalDevice physicalDevice, vk::PhysicalDeviceFeatures2 features)
	{
		features.features = featuresIntersect(physicalDevice, features.features);//intersects the regular features

		vk::PhysicalDeviceFeatures2 deviceFeatures = cloneFeatures2(features);
		physicalDevice.getFeatures2(&features);

		void* featuresPtr = features.pNext;
		void* deviceFeaturesPtr = deviceFeatures.pNext;

		while(featuresPtr != nullptr)
		{
			switch (*((vk::StructureType*)featuresPtr))
			{
			case(vk::StructureType::ePhysicalDeviceVulkan11Features):
				featuresPtr = ((vk::PhysicalDeviceVulkan11Features*)featuresPtr)->pNext;
				deviceFeaturesPtr = ((vk::PhysicalDeviceVulkan11Features*)deviceFeaturesPtr)->pNext;
				break;
			case(vk::StructureType::ePhysicalDeviceVulkan12Features):
				featuresPtr = ((vk::PhysicalDeviceVulkan12Features*)featuresPtr)->pNext;
				deviceFeaturesPtr = ((vk::PhysicalDeviceVulkan12Features*)deviceFeaturesPtr)->pNext;
				break;
			default:
				featuresPtr = nullptr;
				deviceFeaturesPtr = nullptr;
				break;
			}
		}
		return features;
	}
	std::vector<const char*> SmartVkDevice::extensionsIntersect(vk::PhysicalDevice physicalDevice, std::vector<const char*> extensions)
	{
		std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
		std::vector<const char*> intersection;

		bool found = false;
		for (size_t i = 0; i < extensions.size(); i++)
		{
			found = false;
			for (size_t j = 0; j < availableExtensions.size() && !found; j++)
			{
				if (strcmp(extensions.at(i), availableExtensions.at(j).extensionName) == 0)
				{
					intersection.push_back(extensions.at(i));

					found = true;
				}
			}
		}

		return intersection;
	}

	// TODO: finish cloning functions
	vk::PhysicalDeviceFeatures2 SmartVkDevice::cloneFeatures2(vk::PhysicalDeviceFeatures2 features)
	{
		vk::PhysicalDeviceFeatures2 deviceFeatures;

		void* featuresPtr = features.pNext;
		void* deviceFeaturesPtr = deviceFeatures.pNext;

		while (featuresPtr != nullptr)
		{
			switch (*((VkStructureType*)featuresPtr))
			{
			case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES):

				break;
			case(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES):

				break;
			default:

				break;
			}
		}


		return features;
	}
	vk::PhysicalDeviceProperties2 SmartVkDevice::cloneProperties2(vk::PhysicalDeviceProperties2 properties)
	{
		return properties;
	}
	vk::PhysicalDeviceMemoryProperties2 SmartVkDevice::cloneMemoryProperties2(vk::PhysicalDeviceMemoryProperties2 properties)
	{
		return properties;
	}

	bool SmartVkDevice::hasExtensionSupport(vk::PhysicalDevice physicalDevice, std::vector<const char*> extensions)
	{
		std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();

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
	bool SmartVkDevice::hasFeatureSupport(vk::PhysicalDevice physicalDevice, vk::PhysicalDeviceFeatures features)
	{
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		for (int i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++)
		{
			if ((!*((VkBool32*)(&features) + i)) || (*((VkBool32*)(&features) + i) && (*((VkBool32*)(&deviceFeatures) + i))))
			{
				return false;
			}
		}
		return true;
	}

	void SmartVkDevice::enumeratePhysicalDevices()
	{
		if (SmartVkInstance::instance == VK_NULL_HANDLE)
		{
			SmartVkFunctions::error("physical device initilaized before instance");
		}

		if (availableDevices.size() == 0)
		{
			availableDevices = SmartVkInstance::instance.enumeratePhysicalDevices();
			if (availableDevices.size() == 0)
			{
				SmartVkFunctions::error("No physical devices available");
			}
		}
	}
	void SmartVkDevice::enumeratePhysicalDevicesGroups()
	{
		enumeratePhysicalDevices();

		if (availableDeviceGroups.size() == 0)
		{
			availableDeviceGroups = SmartVkInstance::instance.enumeratePhysicalDeviceGroups();
		}
	}

	void SmartVkDevice::getDeviceQueue(SmartVkQueue* queue)
	{
		
	}
	std::vector<vk::PhysicalDevice> SmartVkDevice::getAvailableDevices()
	{
		enumeratePhysicalDevices();
		return availableDevices;
	}
	std::vector<vk::PhysicalDeviceGroupProperties> SmartVkDevice::getAvailableDeviceGroups()
	{
		enumeratePhysicalDevices();
		return availableDeviceGroups;
	}
	std::vector<SmartVkDevice*> SmartVkDevice::getSelfPtrList()
	{
		return selfPtrList;
	}
	
	void SmartVkDevice::printAvailableDevices()
	{
		enumeratePhysicalDevices();

		VkPhysicalDeviceProperties properties;
		for (size_t i = 0; i < availableDevices.size(); i++)
		{
			vkGetPhysicalDeviceProperties(availableDevices.at(i), &properties);
			std::cout << properties.deviceName << '\n';
		}
	}
	void SmartVkDevice::printAvailableExtensions()
	{
		printAvailableExtensions(physicalDevices[0]);
	}
	void SmartVkDevice::printAvailableExtensions(vk::PhysicalDevice device)
	{
		std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties();

		for (size_t i = 0; i < extensions.size(); i++)
		{
			std::cout << extensions.at(i).extensionName << "\n";
		}
	}
	void SmartVkDevice::printDeviceExtensionFeatures(vk::PhysicalDevice device)
	{
	}
}