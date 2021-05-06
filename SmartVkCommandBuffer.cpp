#include "SmartVkCommandBuffer.h"

namespace vk
{
	std::vector<SmartVkCommandBuffer*> SmartVkCommandBuffer::selfPtrList = std::vector<SmartVkCommandBuffer*>();

	SmartVkCommandBuffer::SmartVkCommandBuffer()
		: commandBuffer(nullptr),
		commandBufferLevel(vk::CommandBufferLevel::ePrimary),
		commandPoolPtr(nullptr)
	{
		selfPtrList.push_back(this);
	}
	SmartVkCommandBuffer::SmartVkCommandBuffer(vk::Device* device, vk::CommandBufferLevel level, uint32_t queueIndex)
		: commandBuffer(nullptr),
		commandBufferLevel(vk::CommandBufferLevel::ePrimary),
		commandPoolPtr(nullptr)
	{
		initCommandBuffer(device, level, queueIndex);
		selfPtrList.push_back(this);
	}
	SmartVkCommandBuffer::SmartVkCommandBuffer(SmartVkCommandPool* commandPool, vk::CommandBufferLevel level)
		: commandBuffer(nullptr),
		commandBufferLevel(vk::CommandBufferLevel::ePrimary),
		commandPoolPtr(nullptr)
	{
		initCommandBuffer(commandPool, level);
		selfPtrList.push_back(this);
	}

	//init command buffer currently onely ever inits one command buffer in place of an array, this should be fine the if another buffer is to be allocated, it shouldn't be stoppped by vulkan
	//however it may need changing
	void SmartVkCommandBuffer::initCommandBuffer(vk::Device* device, vk::CommandBufferLevel level, uint32_t queueIndex)
	{
		initCommandBuffer(SmartVkCommandPool::getCommandPool(device, queueIndex), level);
	}
	void SmartVkCommandBuffer::initCommandBuffer(SmartVkCommandPool* commandPool, vk::CommandBufferLevel level)
	{
		commandPoolPtr = commandPool;
		commandBufferLevel = level;

		vk::CommandBufferAllocateInfo createInfo(commandPoolPtr->commandPool, commandBufferLevel, 1);

		SmartVkFunctions::printDebugStr(2, "allocating command buffer");
		commandBuffer = (commandPoolPtr->devicePtr->device.allocateCommandBuffers(createInfo)).at(0);
	}

	void SmartVkCommandBuffer::recordCommandBuffer(vk::CommandBufferUsageFlags flags)
	{
		if (flags & vk::CommandBufferUsageFlagBits::eRenderPassContinue) //
		{
			//warning here? or error?
		}

		vk::CommandBufferInheritanceInfo inheritenceInfo; //decide whether this should be kept? maybe there are two init methods where one takes in a command buffer rather than a level
		vk::DeviceGroupCommandBufferBeginInfo deviceGroupBeginInfo(0);
		vk::CommandBufferBeginInfo beginInfo(flags, nullptr);

		if (commandPoolPtr->devicePtr->physicalDevicesCount > 1)
		{
			deviceGroupBeginInfo.deviceMask = commandPoolPtr->devicePtr->physicalDevicesCount;

			beginInfo.pNext = (void*)&deviceGroupBeginInfo;
		}

		if (commandBufferLevel == vk::CommandBufferLevel::eSecondary)
		{
			//figure out whether this should be done? having just one record command buffer function
			//this likley requiures a change in the whole class, something where it can store information base on whether it is a secondary
			//maybe in the init 

			beginInfo.pInheritanceInfo = &inheritenceInfo;
		}

		commandBuffer.begin(beginInfo);
		SmartVkFunctions::printDebugStr(0, "command buffer recording started", true);
	}
	void SmartVkCommandBuffer::recordCommandBufferSecondary(vk::CommandBufferUsageFlags flags, vk::CommandBufferInheritanceInfo inheretenceInfo)
	{

	}

	void SmartVkCommandBuffer::endCommandBuffer()
	{
		commandBuffer.end();
		SmartVkFunctions::printDebugStr(0, "command buffer recording ended", true);
	}

	void SmartVkCommandBuffer::submitCommandBuffer()
	{
		
		vk::SubmitInfo info(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr);
		commandPoolPtr->queuePtr->queue.submit(1, &info, vk::Fence(), commandPoolPtr->devicePtr->dispatcher);
		SmartVkFunctions::printDebugStr(0, "command buffer submitted", true);
	}

	void SmartVkCommandBuffer::resetCommandBuffer()
	{
		resetCommandBuffer(vk::CommandBufferResetFlags(0));
	}
	void SmartVkCommandBuffer::resetCommandBuffer(vk::CommandBufferResetFlags flags)
	{
		commandBuffer.reset(flags);
	}

	void SmartVkCommandBuffer::freeCommandBuffer()
	{
		commandPoolPtr->devicePtr->device.freeCommandBuffers(commandPoolPtr->commandPool, 1, &commandBuffer);
	}
}
