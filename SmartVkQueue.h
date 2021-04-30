#pragma once

#include "SmartVkIncludes.h"

namespace vk
{
	class SmartVkQueue
	{
	public:
		vk::Queue queue;
		const uint32_t queueId; //a unique number identfying the queue, may remove if not used

		float queuePriority;
		vk::QueueFlags flag;
		float soloQueuePriority; //preference for queue to get solo queue family

		uint32_t queueFamilyIndex;
		bool foundQueue;
		uint32_t indexInQueueFamily;

		SmartVkQueue();
		SmartVkQueue(vk::QueueFlags flags, float queuePriority, float soloQueuePriority);
		virtual ~SmartVkQueue();

		void setQueue(vk::QueueFlags flags, float queuePriority, float soloQueuePriority);

		//void submitCommandBuffer();

		static bool isSoloQueueFlag(vk::QueueFlags flag);

		static uint32_t highestQueueId();

		static std::vector<SmartVkQueue*> getSelfPtrList();
	private:
		static std::vector<SmartVkQueue*> selfPtrList;
	};
}

