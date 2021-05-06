#pragma once

#include "SmartVkIncludes.h"
#include "SmartVkInstance.h"
#include "SmartVkQueue.h"
#include "SmartVkDevice.h"

namespace vk
{
	class SmartVkCommandPool
	{
	public:
		vk::CommandPool commandPool;
		SmartVkDevice* devicePtr;
		SmartVkQueue* queuePtr;

		SmartVkCommandPool();
		SmartVkCommandPool(SmartVkDevice* device, SmartVkQueue* queue, vk::CommandPoolCreateFlags flags);
		virtual ~SmartVkCommandPool();

		void initCommandPool(SmartVkDevice* device, SmartVkQueue* queue);
		void initCommandPool(SmartVkDevice* device, SmartVkQueue* queue, vk::CommandPoolCreateFlags flags);

		static std::vector<SmartVkCommandPool> createCommandPools(SmartVkDevice* device, std::vector<SmartVkQueue*> queuePtrList);

		void trimCommandPool();
		void trimCommandPool(vk::CommandPoolTrimFlags flags);

		void resetCommandPool();
		void resetCommandPool(vk::CommandPoolResetFlags flags);

		static SmartVkCommandPool* getCommandPool(vk::Device* device, uint32_t queueIndex);
	private:
		static std::vector<SmartVkCommandPool*> selfPtrList;
	};
}

