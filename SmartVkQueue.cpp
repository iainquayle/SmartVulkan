#include "SmartVkQueue.h"

namespace vk
{
	std::vector<SmartVkQueue*> SmartVkQueue::selfPtrList;

	SmartVkQueue::SmartVkQueue()
		:queue(nullptr),
		queueId(SmartVkQueue::highestQueueId() + 1),

		queuePriority(1.0f),
		flag(),
		soloQueuePriority(0.0f),

		queueFamilyIndex(0),
		foundQueue(false),
		indexInQueueFamily(0)
	{
		selfPtrList.push_back(this);
	}
	SmartVkQueue::SmartVkQueue(vk::QueueFlags flag, float queuePriority, float soloQueuePriority)
		:queue(nullptr),
		queueId(SmartVkQueue::highestQueueId() + 1),

		queuePriority(queuePriority),
		flag(flag),
		soloQueuePriority(soloQueuePriority),

		queueFamilyIndex(0),
		foundQueue(false),
		indexInQueueFamily(0)
	{
		selfPtrList.push_back(this);
	}
	SmartVkQueue::~SmartVkQueue()
	{
	}

	void SmartVkQueue::setQueue(vk::QueueFlags flag, float queuePriority, float soloQueuePriority)
	{
		this->flag = flag;
		this->queuePriority = queuePriority;
		this->soloQueuePriority = soloQueuePriority;
	}
	
	bool SmartVkQueue::isSoloQueueFlag(vk::QueueFlags flag)
	{
		VkQueueFlags cFlag = (VkQueueFlags)flag;
		while (cFlag % 2 == 0)
		{
			cFlag /= 2;
		}
		return cFlag == 1;
	}

	uint32_t SmartVkQueue::highestQueueId()
	{
		if (selfPtrList.size() == 0)
		{
			return 0;
		}

		uint32_t highestId = 0;
		for (int i = 0; i < selfPtrList.size(); i++)
		{
			if (selfPtrList.at(i)->queueId > highestId)
			{
				highestId = selfPtrList.at(i)->queueId;
			}
		}
		return highestId;
	}

	std::vector<SmartVkQueue*> SmartVkQueue::getSelfPtrList()
	{
		return selfPtrList;
	}
}