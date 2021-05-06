#pragma once

#include "SmartVkIncludes.h"
#include "SmartVkInstance.h"
#include "SmartVkDevice.h"
#include "SmartVkQueue.h"
#include "SmartVkCommandPool.h"


namespace vk
{
	class SmartVkCommandBuffer
	{
	public:
		vk::CommandBuffer commandBuffer;
		vk::CommandBufferLevel commandBufferLevel;
		SmartVkCommandPool* commandPoolPtr;


		SmartVkCommandBuffer();
		//TODO: take out the option to input a device and index, takout level and add init which takes in info required for secondary inheretence info, then it can be infered
		SmartVkCommandBuffer(vk::Device* device, vk::CommandBufferLevel level, uint32_t queueIndex);
		SmartVkCommandBuffer(SmartVkCommandPool* commandPool, vk::CommandBufferLevel level);

		void initCommandBuffer(vk::Device* device, vk::CommandBufferLevel level, uint32_t queueIndex);
		void initCommandBuffer(SmartVkCommandPool* commandPool, vk::CommandBufferLevel level);

		//TODO: change to over loaded functions? or maybe even have enough information kept by the class to make fill out the inheritence struct if required?
		void recordCommandBuffer(vk::CommandBufferUsageFlags flags);
		void recordCommandBufferSecondary(vk::CommandBufferUsageFlags flags, vk::CommandBufferInheritanceInfo inheretenceInfo);

		void endCommandBuffer();

		void submitCommandBuffer();

		void resetCommandBuffer();
		void resetCommandBuffer(vk::CommandBufferResetFlags flags);

		void freeCommandBuffer();
	private:
		static std::vector<SmartVkCommandBuffer*> selfPtrList;
	};
}