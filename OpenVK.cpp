#include "SmartVk.h"

//main is just here for testing purposes
using namespace std;

vk::SmartVkDevice device;
vk::PhysicalDeviceFeatures2 features;

static vk::SmartVkQueue computeQueue(vk::QueueFlags(VK_QUEUE_COMPUTE_BIT), 1.0f, 1.0f);
static vk::SmartVkQueue transferQueue(vk::QueueFlags(VK_QUEUE_TRANSFER_BIT), 1.0f, 1.0f);
static vk::SmartVkQueue presentQueue(vk::QueueFlags(VK_QUEUE_GRAPHICS_BIT), 1.0f, 1.0f);

vk::SmartVkCommandPool computePool;
vk::SmartVkCommandPool transferPool;
vk::SmartVkCommandPool presentPool;



static vector<const char*> instanceExtensions =
{
	VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
};
static vector<const char*> instanceDebugExtensions =
{
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
static vector<const char*> deviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	VK_KHR_SURFACE_EXTENSION_NAME
};
static vector<const char*> deviceDebugExtensions =
{
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
static vector<const char*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

float rateDevice(vk::PhysicalDevice device, std::vector<vk::SmartVkQueue*> queuePtrList, float queueScore);

int main()
{
	vk::SmartVkFunctions::enableDebugMessages = true;
	vk::SmartVkInstance::initInstance("test", "voxo", 1, instanceExtensions, instanceDebugExtensions, validationLayers, true);
	features.features.shaderFloat64 = true;
	features.features.shaderInt64 = true;
	device.pickInitDevice({ &computeQueue, &transferQueue, &presentQueue }, features, &rateDevice, deviceExtensions, deviceDebugExtensions, validationLayers);
	computePool.initCommandPool(&device, &computeQueue);
}

float rateDevice(vk::PhysicalDevice device, std::vector<vk::SmartVkQueue*> queuePtrList, float queueScore)
{
	return queueScore;
}
