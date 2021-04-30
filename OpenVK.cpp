#include "SmartVk.h"

//main is just here for testing purposes
using namespace std;

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
};
static vector<const char*> deviceDebugExtensions =
{
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
static vector<const char*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

static vk::SmartVkQueue computeQueue(vk::QueueFlags(VK_QUEUE_COMPUTE_BIT), 1.0f, 1.0f);
static vk::SmartVkQueue transferQueue(vk::QueueFlags(VK_QUEUE_TRANSFER_BIT), 1.0f, 1.0f);
static vk::SmartVkQueue presentQueue(vk::QueueFlags(VK_QUEUE_GRAPHICS_BIT), 1.0f, 1.0f);

int main()
{
	vk::SmartVkFunctions::enableDebugMessages = true;
	vk::SmartVkInstance::initInstance("test", "voxo", 1, instanceExtensions, instanceDebugExtensions, validationLayers, true);
}


