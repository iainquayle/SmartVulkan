#include "SmartVkCommandPool.h"

namespace vk
{
	std::vector<SmartVkCommandPool*> SmartVkCommandPool::selfPtrList = std::vector<SmartVkCommandPool*>();

	SmartVkCommandPool::SmartVkCommandPool()
		:commandPool(nullptr),
		devicePtr(nullptr),
		queuePtr(nullptr)
	{
		selfPtrList.push_back(this);
	}
	SmartVkCommandPool::SmartVkCommandPool(SmartVkDevice* device, SmartVkQueue* queue, vk::CommandPoolCreateFlags flags)
		:commandPool(nullptr),
		devicePtr(nullptr),
		queuePtr(nullptr)
	{
		initCommandPool(device, queue, flags);
		selfPtrList.push_back(this);
	}
	SmartVkCommandPool::~SmartVkCommandPool()
	{
		vkDestroyCommandPool(devicePtr->device, commandPool, nullptr);
	}

	void SmartVkCommandPool::initCommandPool(SmartVkDevice* device, SmartVkQueue* queue)
	{
		initCommandPool(device, queue, vk::CommandPoolCreateFlags());
	}
	void SmartVkCommandPool::initCommandPool(SmartVkDevice* device, SmartVkQueue* queue, vk::CommandPoolCreateFlags flags)
	{
		devicePtr = device;
		queuePtr = queue;

		vk::CommandPoolCreateInfo createInfo(flags, queuePtr->queueFamilyIndex);
		commandPool = devicePtr->device.createCommandPool(createInfo);

		SmartVkFunctions::printDebugStr(0, "command pool created", true);
	}

	//TODO: wtf was I doing here? 
	//creates command pools with no optional flags, to match all queues that were created
	std::vector<SmartVkCommandPool> SmartVkCommandPool::createCommandPools(SmartVkDevice* device, std::vector<SmartVkQueue*> queuePtrList)
	{
		/*
		std::vector<SmartVkCommandPool> commandPools = std::vector<SmartVkCommandPool>();
		bool foundQueueFamily = false;

		for (size_t i = 0; i < queuePtrList.size(); i++)
		{

			
			foundQueueFamily = false;
			for (size_t j = 0; j < commandPools.size() && !foundQueueFamily; j++)
			{
				foundQueueFamily = (queuePtrList.at(i) == commandPools.at(j).queuePtr);
			}
			if (!foundQueueFamily)
			{
				commandPools.push_back(SmartVkCommandPool(device, queuePtrList.at(i), vk::CommandPoolCreateFlags());
			}
		}
		return commandPools;
		//*/
		return std::vector<SmartVkCommandPool>(0);
	}

	void SmartVkCommandPool::trimCommandPool()
	{
		trimCommandPool(vk::CommandPoolTrimFlags());
	}
	void SmartVkCommandPool::trimCommandPool(vk::CommandPoolTrimFlags flags)
	{
		devicePtr->device.trimCommandPool(commandPool, flags);
	}

	void SmartVkCommandPool::resetCommandPool()
	{
		resetCommandPool(vk::CommandPoolResetFlags());
	}
	void SmartVkCommandPool::resetCommandPool(vk::CommandPoolResetFlags flags)
	{
		devicePtr->device.resetCommandPool(commandPool, flags);
	}

	SmartVkCommandPool* SmartVkCommandPool::getCommandPool(vk::Device* device, uint32_t queueIndex)
	{
		for (size_t i = 0; i < selfPtrList.size(); i++)
		{
			if (selfPtrList.at(i)->devicePtr->device == *device && selfPtrList.at(i)->queuePtr->queueFamilyIndex == queueIndex)
			{
				return selfPtrList.at(i);
			}
		}
		
		std::cout << "no command pool matched the inputed information";
		return nullptr;
	}
}
