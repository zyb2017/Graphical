#include <vulkan/vulkan.h>
#include "../util/MatrixState3D.h"
#include "MyVulkanManager.h"
#include "../util/FileUtil.h"
#include "../util/HelpFunction.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include "ThreadTask.h"
#include "../util/FPSUtil.h"
#include "TriangleData.h"
#include <time.h>
#include "../legencyUtil/util.hpp"

//��ǰʹ�õ�GPU���� ��ֻ��һ��GPU��ֵֻ������0
#define USED_GPU_INDEX 0

//�ڲ�ʹ�õ�����Image���ֵķ���
void setImageLayout(VkCommandBuffer cmd, VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout old_image_layout,
	VkImageLayout new_image_layout)
{

	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = NULL;
	image_memory_barrier.srcAccessMask = 0;
	image_memory_barrier.dstAccessMask = 0;
	image_memory_barrier.oldLayout = old_image_layout;
	image_memory_barrier.newLayout = new_image_layout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.image = image;
	image_memory_barrier.subresourceRange.aspectMask = aspectMask;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;

	if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.srcAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_PREINITIALIZED) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_memory_barrier.srcAccessMask =
			VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}
//========================================����Ϊ�ļ��ڲ�ʹ�õĸ�������=========================
//===================================����Ϊ���ʵ��============================================

//��̬��Աʵ��

struct WindowInfo MyVulkanManager::info;
bool MyVulkanManager::loopDrawFlag = true;
std::vector<const char *>  MyVulkanManager::instanceExtensionNames;
VkInstance MyVulkanManager::instance;
uint32_t MyVulkanManager::gpuCount;
std::vector<VkPhysicalDevice> MyVulkanManager::gpus;
uint32_t MyVulkanManager::queueFamilyCount;
std::vector<VkQueueFamilyProperties> MyVulkanManager::queueFamilyprops;
uint32_t MyVulkanManager::queueGraphicsFamilyIndex;
VkQueue MyVulkanManager::queueGraphics;
uint32_t MyVulkanManager::queuePresentFamilyIndex;
std::vector<const char *> MyVulkanManager::deviceExtensionNames;
VkDevice MyVulkanManager::device;
VkCommandPool MyVulkanManager::cmdPool;
VkCommandBuffer MyVulkanManager::cmdBuffer;
VkCommandBufferBeginInfo MyVulkanManager::cmd_buf_info;
VkCommandBuffer  MyVulkanManager::cmd_bufs[1];
VkSubmitInfo MyVulkanManager::submit_info[1];
uint32_t MyVulkanManager::screenWidth;
uint32_t MyVulkanManager::screenHeight;
VkSurfaceKHR MyVulkanManager::surface;
std::vector<VkFormat> MyVulkanManager::formats;
VkSurfaceCapabilitiesKHR MyVulkanManager::surfCapabilities;
uint32_t MyVulkanManager::presentModeCount;
std::vector<VkPresentModeKHR> MyVulkanManager::presentModes;
VkExtent2D MyVulkanManager::swapchainExtent;
VkSwapchainKHR MyVulkanManager::swapChain;
uint32_t MyVulkanManager::swapchainImageCount;
std::vector<VkImage> MyVulkanManager::swapchainImages;
std::vector<VkImageView> MyVulkanManager::swapchainImageViews;
VkFormat MyVulkanManager::depthFormat;
VkFormatProperties MyVulkanManager::depthFormatProps;
VkImage MyVulkanManager::depthImage;
VkPhysicalDeviceMemoryProperties MyVulkanManager::memoryroperties;
VkDeviceMemory MyVulkanManager::memDepth;
VkImageView MyVulkanManager::depthImageView;
VkSemaphore MyVulkanManager::imageAcquiredSemaphore;
uint32_t MyVulkanManager::currentBuffer;
VkRenderPass MyVulkanManager::renderPass;
VkClearValue MyVulkanManager::clear_values[2];
VkRenderPassBeginInfo MyVulkanManager::rp_begin;
VkFence MyVulkanManager::taskFinishFence;
VkPresentInfoKHR MyVulkanManager::present;

VkFramebuffer*  MyVulkanManager::framebuffers;

ShaderQueueSuit_Common* MyVulkanManager::sqsCL;
DrawableObjectCommonLight*  MyVulkanManager::triForDraw;
float MyVulkanManager::xAngle = 0;
float MyVulkanManager::yAngle = 0;
float MyVulkanManager::zAngle = 0;


//����vulkanʵ���ķ���
void MyVulkanManager::init_vulkan_instance()
{
	
	//��ʼ��������չ�б�����������չ��Android������Ⱦ����Ҫ��
	instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);//�˴���ֲWindows�������
	instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);//�˴���ֲWindows��Ҫ��ȡVK_KHR_SURFACE_EXTENSION_NAME��չ

	VkApplicationInfo app_info = {};//����Ӧ����Ϣ�ṹ��ʵ��
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;//�ṹ�������
	app_info.pNext = NULL;//�Զ������ݵ�ָ��
	app_info.pApplicationName = "HelloVulkan";//Ӧ�õ�����
	app_info.applicationVersion = 1;//Ӧ�õİ汾��
	app_info.pEngineName = "HelloVulkan";//Ӧ�õ���������
	app_info.engineVersion = 1;//Ӧ�õ�����汾��
	app_info.apiVersion = VK_API_VERSION_1_0;//ʹ�õ�Vulkanͼ��Ӧ�ó���API�汾

	VkInstanceCreateInfo inst_info = {};//����ʵ��������Ϣ�ṹ��ʵ��
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;//�ṹ�������
	inst_info.pNext = NULL;//�Զ������ݵ�ָ��
	inst_info.flags = 0;//������ʹ�õı�־
	inst_info.pApplicationInfo = &app_info;//��Ӧ����Ϣ�ṹ��
	inst_info.enabledExtensionCount = instanceExtensionNames.size();//��չ������
	inst_info.ppEnabledExtensionNames = instanceExtensionNames.data();//��չ�����б�����
	inst_info.enabledLayerCount = 0;//�����Ĳ�����
	inst_info.ppEnabledLayerNames = NULL;//�����Ĳ������б�

	VkResult result;//�洢���н���ĸ�������

	//����Vulkanʵ��
	result = vkCreateInstance(&inst_info, NULL, &instance);//����ʵ��
	if (result == VK_SUCCESS)
	{
		printf("Vulkanʵ�������ɹ�!\n");
	}
	else
	{
		printf("Vulkanʵ������ʧ��!\n");
	}
}

//����vulkanʵ���ķ���
void MyVulkanManager::destroy_vulkan_instance()
{
	vkDestroyInstance(instance, NULL);
	printf("Vulkanʵ���������!\n");
	destroy_window(info);
}

//��ȡӲ���豸�ķ�����GPU��
void MyVulkanManager::enumerate_vulkan_phy_devices()
{
	gpuCount = 0;//�洢�����豸�����ı���
	VkResult result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);//��ȡ�����豸����
	assert(result == VK_SUCCESS);
	printf("[VulkanӲ���豸����Ϊ%d��]", gpuCount);
	gpus.resize(gpuCount);//���������豸�б�ߴ�
	result =vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());//��������豸�б�
	assert(result == VK_SUCCESS);
	vkGetPhysicalDeviceMemoryProperties(gpus[USED_GPU_INDEX], &memoryroperties);//��ȡ��һ�����豸���ڴ�����
}

//�����߼��豸�ķ���
void MyVulkanManager::create_vulkan_devices()
{
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, NULL);//��ȡ�����豸0�ж��м��������
	printf("[VulkanӲ���豸0֧�ֵĶ��м�������Ϊ%d]\n", queueFamilyCount);
	queueFamilyprops.resize(queueFamilyCount);//����м��������ı�vector����
	vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queueFamilyCount, queueFamilyprops.data());//��������豸0���м��������б�
	printf("[�ɹ���ȡVulkanӲ���豸0֧�ֵĶ��м��������б�]\n");

	VkDeviceQueueCreateInfo queueInfo = {};//�����豸���д�����Ϣ�ṹ��ʵ��
	bool found = false;//������־
	for (unsigned int i = 0; i < queueFamilyCount; i++) {//�������ж��м���
		if (queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {//����ǰ���м���֧��ͼ�ι���
			queueInfo.queueFamilyIndex = i;//�󶨴˶��м�������
			queueGraphicsFamilyIndex = i;//��¼֧��ͼ�ι����Ķ��м�������
			printf("[֧��GRAPHICS������һ�����м��������Ϊ%d]\n", i);
			printf("[�˼����е�ʵ�ʶ���������%d]\n", queueFamilyprops[i].queueCount);
			found = true;
			break;
		}
	}

	float queue_priorities[1] = { 0.0 };//�����������ȼ�����
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;//�����ṹ������
	queueInfo.pNext = NULL;//�Զ������ݵ�ָ��
	queueInfo.queueCount = 1;//ָ����������
	queueInfo.pQueuePriorities = queue_priorities;//����ÿ�����е����ȼ�
	queueInfo.queueFamilyIndex = queueGraphicsFamilyIndex;//�󶨶��м�������
	deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);//����������չ

	VkDeviceCreateInfo deviceInfo = {};//�����߼��豸������Ϣ�ṹ��ʵ��
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;//�����ṹ������
	deviceInfo.pNext = NULL;//�Զ������ݵ�ָ��
	deviceInfo.queueCreateInfoCount = 1;//ָ���豸���д�����Ϣ�ṹ������
	deviceInfo.pQueueCreateInfos = &queueInfo;//�����豸���д�����Ϣ�ṹ���б�
	deviceInfo.enabledExtensionCount = deviceExtensionNames.size();//������չ����
	deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();//������չ�б�
	deviceInfo.enabledLayerCount = 0;//������Layer������
	deviceInfo.ppEnabledLayerNames = NULL;//������Layer�������б�
	deviceInfo.pEnabledFeatures = NULL;//���õ��豸����
	VkResult result = vkCreateDevice(gpus[0], &deviceInfo, NULL, &device);//�����߼��豸
	assert(result == VK_SUCCESS);//����߼��豸�Ƿ񴴽��ɹ�
}

//�����߼��豸�ķ���
void MyVulkanManager::destroy_vulkan_devices()
{
	vkDestroyDevice(device, NULL);
	printf("�߼��豸������ϣ�\n");
}

//���������ķ���
void MyVulkanManager::create_vulkan_CommandBuffer()
{
	VkCommandPoolCreateInfo cmd_pool_info = {};//��������ش�����Ϣ�ṹ��ʵ��
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 	//�����ṹ������
	cmd_pool_info.pNext = NULL;//�Զ������ݵ�ָ��
	cmd_pool_info.queueFamilyIndex = queueGraphicsFamilyIndex;//��������м�������
	cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//ִ�п��Ʊ�־
	VkResult result = vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmdPool);//���������
	assert(result == VK_SUCCESS);//�������ش����Ƿ�ɹ�

	VkCommandBufferAllocateInfo cmdBAI = {};//��������������Ϣ�ṹ��ʵ��
	cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;//�����ṹ������
	cmdBAI.pNext = NULL;//�Զ������ݵ�ָ��
	cmdBAI.commandPool = cmdPool;//ָ�������
	cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//���������弶��
	cmdBAI.commandBufferCount = 1;//��������������
	result = vkAllocateCommandBuffers(device, &cmdBAI, &cmdBuffer);//���������

	assert(result == VK_SUCCESS);//�������Ƿ�ɹ�
	cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//�����ṹ������
	cmd_buf_info.pNext = NULL;//�Զ������ݵ�ָ��
	cmd_buf_info.flags = 0;//����ʹ�ñ�־
	cmd_buf_info.pInheritanceInfo = NULL;//�����̳���Ϣ
	cmd_bufs[0] = cmdBuffer;//Ҫ�ύ������ִ�е����������

	VkPipelineStageFlags* pipe_stage_flags = new VkPipelineStageFlags();//Ŀ����߽׶�
	*pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info[0].pNext = NULL;//�Զ������ݵ�ָ��
	submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;//�����ṹ������
	submit_info[0].pWaitDstStageMask = pipe_stage_flags;//����Ŀ����߽׶�
	submit_info[0].commandBufferCount = 1;//���������
	submit_info[0].pCommandBuffers = cmd_bufs;//�ύ�����������
	submit_info[0].signalSemaphoreCount = 0;//�ź�������
	submit_info[0].pSignalSemaphores = NULL;//�ź�������
}

void MyVulkanManager::destroy_vulkan_CommandBuffer()//���������ķ���
{
	//����Ҫ�ͷŵ����������
	VkCommandBuffer cmdBufferArray[1] = { cmdBuffer };
	//�ͷ������
	vkFreeCommandBuffers
	(
		device, //�����߼��豸
		cmdPool,//���������
		1,      //Ҫ���ٵ����������
		cmdBufferArray//Ҫ���ٵ����������
	);
	//���������
	vkDestroyCommandPool(device, cmdPool, NULL);
}

//����������swapChain�ķ���
void MyVulkanManager::create_vulkan_swapChain()
{
	//����KHR���洴����Ϣ�ṹ��ʵ��
	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;//�����ṹ������
	createInfo.pNext = nullptr;//�Զ������ݵ�ָ��
	createInfo.flags = 0;//��δ��ʹ�õı�־
	createInfo.hwnd = info.window;//��������

	//����KHR����
	VkResult result=vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
	assert(result == VK_SUCCESS);

	//�����豸��Ӧ�Ķ��м����б�
	VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(gpus[USED_GPU_INDEX], i, surface, &pSupportsPresent[i]);
		printf("���м�������=%d %s��ʾ\n", i, (pSupportsPresent[i] == 1 ? "֧��" : "��֧��"));
	}

	queueGraphicsFamilyIndex = UINT32_MAX;//֧��ͼ�ι����Ķ��м�������
	queuePresentFamilyIndex = UINT32_MAX;//֧����ʾ(����)�����Ķ��м�������
	for (uint32_t i = 0; i <queueFamilyCount; ++i)//�����豸��Ӧ�Ķ��м����б�
	{
		//�����ǰ�������Ķ��м���֧��Graphis��ͼ�Σ�����
		if ((queueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)//���˶��м���֧��ͼ�ι���
		{
			//��¼֧��Graphis��ͼ�Σ������Ķ��м�������
			if (queueGraphicsFamilyIndex == UINT32_MAX) queueGraphicsFamilyIndex = i;
			//�����ǰ�������Ķ��м���֧��Present����ʾ����������
			if (pSupportsPresent[i] == VK_TRUE)//�����ǰ���м���֧����ʾ����
			{
				queueGraphicsFamilyIndex = i;//��¼�˶��м�������Ϊ֧��ͼ�ι�����
				queuePresentFamilyIndex = i;//��¼�˶��м�������Ϊ֧����ʾ������
				printf("���м�������=%d ͬʱ֧��Graphis��ͼ�Σ���Present����ʾ������\n", i);
				break;
			}
		}
	}

	if (queuePresentFamilyIndex == UINT32_MAX)//��û���ҵ�ͬʱ֧��������Ķ��м���
	{
		for (size_t i = 0; i < queueFamilyCount; ++i)//�����豸��Ӧ�Ķ��м����б�
		{
			if (pSupportsPresent[i] == VK_TRUE)//�ж��Ƿ�֧����ʾ����
			{
				queuePresentFamilyIndex = i;//��¼�˶��м�������Ϊ֧����ʾ������
				break;
			}
		}
	}
	free(pSupportsPresent);//�ͷŴ洢�Ƿ�֧�ֳ��ֹ����Ĳ���ֵ�б�

	//û���ҵ�֧��Graphis��ͼ�Σ���Present����ʾ�������Ķ��м���
	//û���ҵ�֧��ͼ�λ���ʾ�����Ķ��м���
	if (queueGraphicsFamilyIndex == UINT32_MAX || queuePresentFamilyIndex == UINT32_MAX)
	{
		printf("û���ҵ�֧��Graphis��ͼ�Σ���Present����ʾ�������Ķ��м���\n");
		assert(false);//��û��֧��ͼ�λ���ʾ�����Ķ��м����������ֹ
	}

	uint32_t formatCount;//֧�ֵĸ�ʽ����
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, NULL);//��ȡ֧�ֵĸ�ʽ����
	printf("֧�ֵĸ�ʽ����Ϊ %d\n", formatCount);
	VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));//�����Ӧ�����Ŀռ�
	formats.resize(formatCount);//������ӦVector�ߴ�
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(gpus[0], surface, &formatCount, surfFormats);//��ȡ֧�ֵĸ�ʽ��Ϣ
	for (unsigned int i = 0; i<formatCount; i++) {//��¼֧�ֵĸ�ʽ��Ϣ
		formats[i] = surfFormats[i].format;
		printf("[%d]֧�ֵĸ�ʽΪ%d\n", i, formats[i]);
	}
	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {//�����������
		formats[0] = VK_FORMAT_B8G8R8A8_UNORM;
	}
	free(surfFormats);

	//��ȡKHR���������
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpus[USED_GPU_INDEX], surface, &surfCapabilities);
	assert(result == VK_SUCCESS);

	//��ȡ֧�ֵ���ʾģʽ����
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[USED_GPU_INDEX], surface, &presentModeCount, NULL);
	assert(result == VK_SUCCESS);
	printf("��ʾģʽ����Ϊ%d\n", presentModeCount);

	//������ӦVector�ߴ�
	presentModes.resize(presentModeCount);
	//��ȡ֧�ֵ���ʾģʽ�б�
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(gpus[USED_GPU_INDEX], surface, &presentModeCount, presentModes.data());
	for (unsigned int i = 0; i<presentModeCount; i++)
	{
		printf("��ʾģʽ[%d]���Ϊ%d\n", i, presentModes[i]);
	}

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;//ȷ����������ʾģʽ
	for (size_t i = 0; i < presentModeCount; i++)//������ʾģʽ�б�
	{
		//���Ҳ֧��VK_PRESENT_MODE_MAILBOX_KHRģʽ��������Ч�ʸߣ���ѡ��
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
		{
			//���û������VK_PRESENT_MODE_MAILBOX_KHRģʽ������VK_PRESENT_MODE_IMMEDIATE_KHRģʽ
			//Ҳ��VK_PRESENT_MODE_FIFO_KHRģʽǿ����ѡ��
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	//ȷ��surface�Ŀ�ȡ��߶�
	//���surface�����еĳߴ�û�ж��壨���Ϊ0xFFFFFFFF��ʾû���壩
	if (surfCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		swapchainExtent.width = screenWidth;//���ÿ��Ϊ������
		swapchainExtent.height = screenHeight;//���ø߶�Ϊ����߶�
		//�������ֵ���Ƶ����ֵ����Сֵ֮��
		if (swapchainExtent.width < surfCapabilities.minImageExtent.width)
		{
			swapchainExtent.width = surfCapabilities.minImageExtent.width;
		}
		else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width)
		{
			swapchainExtent.width = surfCapabilities.maxImageExtent.width;
		}
		//�߶�����ֵ���Ƶ����ֵ����Сֵ֮��
		if (swapchainExtent.height < surfCapabilities.minImageExtent.height)
		{
			swapchainExtent.height = surfCapabilities.minImageExtent.height;
		}
		else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height)
		{
			swapchainExtent.height = surfCapabilities.maxImageExtent.height;
		}
		printf("ʹ���Լ����õ� ��� %d �߶� %d\n", swapchainExtent.width, swapchainExtent.height);
	}
	else
	{
		//��������ȷ���ߴ�
		swapchainExtent = surfCapabilities.currentExtent;
		printf("ʹ�û�ȡ��surface�����е� ��� %d �߶� %d\n", swapchainExtent.width, swapchainExtent.height);
	}

	screenWidth = swapchainExtent.width;//��¼ʵ�ʲ��õĿ��
	screenHeight = swapchainExtent.height;//��¼ʵ�ʲ��õĸ߶�

	//�����������е�����ͼ������
	uint32_t desiredMinNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;
	//���������Ƶ���Χ��
	if ((surfCapabilities.maxImageCount > 0) && (desiredMinNumberOfSwapChainImages > surfCapabilities.maxImageCount))
	{
		desiredMinNumberOfSwapChainImages = surfCapabilities.maxImageCount;
	}

	//KHR����任��־
	VkSurfaceTransformFlagBitsKHR preTransform;
	if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)//��֧������ı任
	{
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else//����֧�ֱ任
	{
		preTransform = surfCapabilities.currentTransform;
	}

	VkSwapchainCreateInfoKHR swapchain_ci = {};//����������������Ϣ�ṹ��ʵ��
	swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;//�ṹ������
	swapchain_ci.pNext = NULL;//�Զ������ݵ�ָ��
	swapchain_ci.surface = surface;//ָ��KHR����
	swapchain_ci.minImageCount = desiredMinNumberOfSwapChainImages;//����ͼ������
	swapchain_ci.imageFormat = formats[0];//ͼ���ʽ
	swapchain_ci.imageExtent.width = swapchainExtent.width;//������ͼ����
	swapchain_ci.imageExtent.height = swapchainExtent.height;//������ͼ��߶�
	swapchain_ci.preTransform = preTransform;//ָ���任��־
	swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;//���Alphaֵ
	swapchain_ci.imageArrayLayers = 1;//ͼ���������
	swapchain_ci.presentMode = swapchainPresentMode;//����������ʾģʽ
	swapchain_ci.oldSwapchain = VK_NULL_HANDLE;//ǰ��������
	swapchain_ci.clipped = true;//��������
	swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;//ɫ�ʿռ�
	swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//ͼ����;
	swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;	//ͼ����ģʽ
	swapchain_ci.queueFamilyIndexCount = 0;//���м�������
	swapchain_ci.pQueueFamilyIndices = NULL;//���м��������б�

	if (queueGraphicsFamilyIndex != queuePresentFamilyIndex)//��֧��ͼ�κ���ʾ�����Ķ��м��岻��ͬ
	{
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_ci.queueFamilyIndexCount = 2;//����������Ķ��м�����������Ϊ2
		uint32_t queueFamilyIndices[2] = { queueGraphicsFamilyIndex,queuePresentFamilyIndex };
		swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;//����������Ķ��м��������б�
	}

	result = vkCreateSwapchainKHR(device, &swapchain_ci, NULL, &swapChain);//����������
	assert(result == VK_SUCCESS);//��齻�����Ƿ񴴽��ɹ�

	 //��ȡ�������е�ͼ������
	result = vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, NULL);
	assert(result == VK_SUCCESS);
	printf("[SwapChain�е�Image����Ϊ%d]\n", swapchainImageCount);//����Ƿ��ȡ�ɹ�
	swapchainImages.resize(swapchainImageCount);//����ͼ���б�ߴ�
	//��ȡ�������е�ͼ���б�
	result =vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, swapchainImages.data());
	assert(result == VK_SUCCESS);
	swapchainImageViews.resize(swapchainImageCount);//����ͼ����ͼ�б�ߴ�
	for (uint32_t i = 0; i < swapchainImageCount; i++)//Ϊ�������еĸ���ͼ�񴴽�ͼ����ͼ
	{
		VkImageViewCreateInfo color_image_view = {};//����ͼ����ͼ������Ϣ�ṹ��ʵ��
		color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//���ýṹ������
		color_image_view.pNext = NULL;//�Զ������ݵ�ָ��
		color_image_view.flags = 0;//������ʹ�õı�־
		color_image_view.image = swapchainImages[i];//��Ӧ������ͼ��
		color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;//ͼ����ͼ������
		color_image_view.format = formats[0];//ͼ����ͼ��ʽ
		color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;//����Rͨ������
		color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;//����Gͨ������
		color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;//����Bͨ������
		color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;//����Aͨ������
		color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;//ͼ����ͼʹ�÷���
		color_image_view.subresourceRange.baseMipLevel = 0;//����Mipmap����
		color_image_view.subresourceRange.levelCount = 1;//Mipmap���������
		color_image_view.subresourceRange.baseArrayLayer = 0;//���������
		color_image_view.subresourceRange.layerCount = 1;//����������
		result = vkCreateImageView(device, &color_image_view, NULL, &swapchainImageViews[i]);//����ͼ����ͼ
		assert(result == VK_SUCCESS);//����Ƿ񴴽��ɹ�
	}
}

void MyVulkanManager::destroy_vulkan_swapChain()//���ٽ�������صķ���
{
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkDestroyImageView(device, swapchainImageViews[i], NULL);
		printf("[����SwapChain ImageView %d �ɹ�]\n", i);
	}
	vkDestroySwapchainKHR(device, swapChain, NULL);
	printf("����SwapChain�ɹ�!\n");
}

//������Ȼ������
void MyVulkanManager::create_vulkan_DepthBuffer()
{
	depthFormat = VK_FORMAT_D16_UNORM;//ָ�����ͼ��ĸ�ʽ
	VkImageCreateInfo image_info = {};//�������ͼ�񴴽���Ϣ�ṹ��ʵ��
	vkGetPhysicalDeviceFormatProperties(gpus[0], depthFormat, &depthFormatProps);//��ȡ�����豸֧�ֵ�ָ����ʽ������
	//ȷ��ƽ�̷�ʽ
	if (depthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)//�Ƿ�֧��������Ƭ��֯��ʽ
	{
		image_info.tiling = VK_IMAGE_TILING_LINEAR;//����������Ƭ��֯��ʽ
		printf("tilingΪVK_IMAGE_TILING_LINEAR��\n");
	}
	else if (depthFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)//�Ƿ�֧��������Ƭ��֯��ʽ
	{
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;//����������Ƭ��֯��ʽ
		printf("tilingΪVK_IMAGE_TILING_OPTIMAL��\n");
	}
	else
	{
		printf("��֧��VK_FORMAT_D16_UNORM��\n");//��ӡ��֧��ָ����ʽ����ʾ��Ϣ
	}
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;	//ָ���ṹ������
	image_info.pNext = NULL;//�Զ������ݵ�ָ��
	image_info.imageType = VK_IMAGE_TYPE_2D;//ͼ������
	image_info.format = depthFormat;//ͼ���ʽ
	image_info.extent.width = screenWidth;//ͼ����
	image_info.extent.height = screenHeight;//ͼ��߶�
	image_info.extent.depth = 1;//ͼ�����
	image_info.mipLevels = 1;//ͼ��mipmap����
	image_info.arrayLayers = 1;//ͼ�����������
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;//����ģʽ
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//��ʼ����
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;//ͼ����;
	image_info.queueFamilyIndexCount = 0;//���м�������
	image_info.pQueueFamilyIndices = NULL;//���м��������б�
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//����ģʽ
	image_info.flags = 0;//��־

	VkMemoryAllocateInfo mem_alloc = {};//�����ڴ������Ϣ�ṹ��ʵ��
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//�ṹ������
	mem_alloc.pNext = NULL;//�Զ������ݵ�ָ��
	mem_alloc.allocationSize = 0;//������ڴ��ֽ���
	mem_alloc.memoryTypeIndex = 0;//�ڴ����������

	VkImageViewCreateInfo view_info = {};//�������ͼ����ͼ������Ϣ�ṹ��ʵ��
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//���ýṹ������
	view_info.pNext = NULL;//�Զ������ݵ�ָ��
	view_info.image = VK_NULL_HANDLE;//��Ӧ��ͼ��
	view_info.format = depthFormat;//ͼ����ͼ�ĸ�ʽ
	view_info.components.r = VK_COMPONENT_SWIZZLE_R;//����Rͨ������
	view_info.components.g = VK_COMPONENT_SWIZZLE_G;//����Gͨ������
	view_info.components.b = VK_COMPONENT_SWIZZLE_B;//����Bͨ������
	view_info.components.a = VK_COMPONENT_SWIZZLE_A;//����Aͨ������
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;//ͼ����ͼʹ�÷���
	view_info.subresourceRange.baseMipLevel = 0;//����Mipmap����
	view_info.subresourceRange.levelCount = 1;//Mipmap���������
	view_info.subresourceRange.baseArrayLayer = 0;//���������
	view_info.subresourceRange.layerCount = 1;//����������
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;//ͼ����ͼ������
	view_info.flags = 0;//��־

	VkResult result = vkCreateImage(device, &image_info, NULL, &depthImage);//�������ͼ��
	assert(result == VK_SUCCESS);

	VkMemoryRequirements mem_reqs;//��ȡͼ���ڴ�����
	vkGetImageMemoryRequirements(device, depthImage, &mem_reqs);
	mem_alloc.allocationSize = mem_reqs.size;//��ȡ�����ڴ��ֽ���
	VkFlags requirements_mask = 0;//��Ҫ���ڴ���������
	bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);//��ȡ�����ڴ���������
	assert(flag);//����ȡ�Ƿ�ɹ�
	printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d\n", mem_alloc.memoryTypeIndex);
	result = vkAllocateMemory(device, &mem_alloc, NULL, &memDepth);	//�����ڴ�
	assert(result == VK_SUCCESS);
	result = vkBindImageMemory(device, depthImage, memDepth, 0);//��ͼ����ڴ�
	assert(result == VK_SUCCESS);
	view_info.image = depthImage;//ָ��ͼ����ͼ��Ӧͼ��
	result = vkCreateImageView(device, &view_info, NULL, &depthImageView);//�������ͼ����ͼ
	assert(result == VK_SUCCESS);
}

void MyVulkanManager::destroy_vulkan_DepthBuffer()//������Ȼ������
{
	vkDestroyImageView(device, depthImageView, NULL);
	vkDestroyImage(device, depthImage, NULL);
	vkFreeMemory(device, memDepth, NULL);
	printf("������Ȼ�����سɹ�!\n");
}

//������Ⱦͨ��
void MyVulkanManager::create_render_pass()
{
	VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;		//�����ź���������Ϣ�ṹ��ʵ��
	imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;//�ṹ������
	imageAcquiredSemaphoreCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	imageAcquiredSemaphoreCreateInfo.flags = 0;//������ʹ�õı�־

	VkResult result = vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);//�����ź���
	assert(result == VK_SUCCESS);//����ź����Ƿ񴴽��ɹ�

	VkAttachmentDescription attachments[2];//����������Ϣ����
	attachments[0].format = formats[0];//������ɫ�����ĸ�ʽ
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;//���ò���ģʽ
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ʱ�Ը����Ĳ���
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;//�洢ʱ�Ը����Ĳ���
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//ģ�����ʱ�Ը����Ĳ���
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//ģ��洢ʱ�Ը����Ĳ���
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//��ʼ�Ĳ���
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//����ʱ�����ղ���
	attachments[0].flags = 0;//����λ����
	attachments[1].format = depthFormat;//������ȸ����ĸ�ʽ
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;//���ò���ģʽ
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ʱ�Ը����Ĳ���
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//�洢ʱ�Ը����Ĳ���
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//ģ�����ʱ�Ը����Ĳ���
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//ģ��洢ʱ�Ը����Ĳ���
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 	//��ʼ�Ĳ���
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//����ʱ�Ĳ���
	attachments[1].flags = 0;//����λ����

	VkAttachmentReference color_reference = {};//��ɫ��������
	color_reference.attachment = 0;//��Ӧ����������Ϣ�����±�
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//���ø�������

	VkAttachmentReference depth_reference = {};//��ȸ�������
	depth_reference.attachment = 1;//��Ӧ����������Ϣ�����±�
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//���ø�������

	VkSubpassDescription subpass = {};//������Ⱦ��ͨ�������ṹ��ʵ��
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;//���ù��߰󶨵�
	subpass.flags = 0;//��������
	subpass.inputAttachmentCount = 0;//���븽������
	subpass.pInputAttachments = NULL;//���븽���б�
	subpass.colorAttachmentCount = 1;//��ɫ��������
	subpass.pColorAttachments = &color_reference;//��ɫ�����б�
	subpass.pResolveAttachments = NULL;//Resolve����
	subpass.pDepthStencilAttachment = &depth_reference;//���ģ�帽��
	subpass.preserveAttachmentCount = 0;//preserve��������
	subpass.pPreserveAttachments = NULL;//preserve�����б�

	VkRenderPassCreateInfo rp_info = {};//������Ⱦͨ��������Ϣ�ṹ��ʵ��
	rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;//�ṹ������
	rp_info.pNext = NULL;//�Զ������ݵ�ָ��
	rp_info.attachmentCount = 2;//����������
	rp_info.pAttachments = attachments;//�����б�
	rp_info.subpassCount = 1;//��Ⱦ��ͨ������
	rp_info.pSubpasses = &subpass;//��Ⱦ��ͨ���б�
	rp_info.dependencyCount = 0;//��ͨ����������
	rp_info.pDependencies = NULL;//��ͨ�������б�

	result = vkCreateRenderPass(device, &rp_info, NULL, &renderPass);//������Ⱦͨ��
	assert(result == VK_SUCCESS);

	clear_values[0].color.float32[0] = 0.2f;//֡���������R����ֵ
	clear_values[0].color.float32[1] = 0.2f;//֡���������G����ֵ
	clear_values[0].color.float32[2] = 0.2f;//֡���������B����ֵ
	clear_values[0].color.float32[3] = 0.2f;//֡���������A����ֵ
	clear_values[1].depthStencil.depth = 1.0f;//֡������������ֵ
	clear_values[1].depthStencil.stencil = 0;//֡���������ģ��ֵ

	rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;//��Ⱦͨ��������Ϣ�ṹ������
	rp_begin.pNext = NULL;//�Զ������ݵ�ָ��
	rp_begin.renderPass = renderPass;//ָ��Ҫ��������Ⱦͨ��
	rp_begin.renderArea.offset.x = 0;//��Ⱦ������ʼX����
	rp_begin.renderArea.offset.y = 0;//��Ⱦ������ʼY����
	rp_begin.renderArea.extent.width = screenWidth;//��Ⱦ������
	rp_begin.renderArea.extent.height = screenHeight;//��Ⱦ����߶�
	rp_begin.clearValueCount = 2;//֡�������ֵ����
	rp_begin.pClearValues = clear_values;//֡�������ֵ����
}

void MyVulkanManager::destroy_render_pass()//������Ⱦͨ�����
{
	vkDestroyRenderPass(device, renderPass, NULL);
	vkDestroySemaphore(device, imageAcquiredSemaphore, NULL);
}

//��ȡ�豸��֧��ͼ�ι����Ķ���
void MyVulkanManager::init_queue()
{
	//��ȡָ������������Ϊ0�Ķ���
	vkGetDeviceQueue(device, queueGraphicsFamilyIndex, 0, &queueGraphics);
}

//����֡����
void MyVulkanManager::create_frame_buffer()
{
	VkImageView attachments[2];//����ͼ����ͼ����
	attachments[1] = depthImageView;//�������ͼ����ͼ

	VkFramebufferCreateInfo fb_info = {};//����֡���崴����Ϣ�ṹ��ʵ��
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;//�ṹ������
	fb_info.pNext = NULL;//�Զ������ݵ�ָ��
	fb_info.renderPass = renderPass;//ָ����Ⱦͨ��
	fb_info.attachmentCount = 2;//��������
	fb_info.pAttachments = attachments;//����ͼ����ͼ����
	fb_info.width = screenWidth;//���
	fb_info.height = screenHeight;//�߶�
	fb_info.layers = 1;//����

	uint32_t i;//ѭ�����Ʊ���
	framebuffers = (VkFramebuffer *)malloc(swapchainImageCount * sizeof(VkFramebuffer));//Ϊ֡�������ж�̬�����ڴ�
	assert(framebuffers);//����ڴ�����Ƿ�ɹ�
	//�����������еĸ���ͼ��
	for (i = 0; i < swapchainImageCount; i++)
	{
		attachments[0] = swapchainImageViews[i];//������ɫ������Ӧͼ����ͼ
		VkResult result = vkCreateFramebuffer(device, &fb_info, NULL, &framebuffers[i]);//����֡����
		assert(result == VK_SUCCESS);//����Ƿ񴴽��ɹ�
		printf("[����֡����%d�ɹ���]\n", i);
	}
}

//����֡����
void MyVulkanManager::destroy_frame_buffer()
{
	//ѭ�����ٽ������и���ͼ���Ӧ��֡����
	for (unsigned int i = 0; i < swapchainImageCount; i++)
	{
		vkDestroyFramebuffer(device, framebuffers[i], NULL);
	}
	free(framebuffers);
	printf("����֡����ɹ���\n");
}

//��������������
void MyVulkanManager::createDrawableObject()
{
	TriangleData::genVertexData();//������ɫ�����ζ������ݺ���ɫ����
	triForDraw = new DrawableObjectCommonLight(TriangleData::vdata, TriangleData::dataByteCount, TriangleData::vCount, device, memoryroperties);//������������ɫ�����ζ���
}

//���ٻ���������
void MyVulkanManager::destroyDrawableObject()
{
	delete triForDraw;
}

void MyVulkanManager::createFence()
{
	VkFenceCreateInfo fenceInfo;//դ��������Ϣ�ṹ��ʵ��
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;//�ṹ������
	fenceInfo.pNext = NULL;//�Զ������ݵ�ָ��
	fenceInfo.flags = 0;//������ʹ�õı�־λ
	vkCreateFence(device, &fenceInfo, NULL, &taskFinishFence);//����ʱդ��
}

void MyVulkanManager::destroyFence()
{
	vkDestroyFence(device, taskFinishFence, NULL);
}

void MyVulkanManager::initPresentInfo()
{
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;	//�ṹ������
	present.pNext = NULL;//�Զ������ݵ�ָ��
	present.swapchainCount = 1;//������������
	present.pSwapchains = &swapChain;//�������б�
	present.waitSemaphoreCount = 0;//�ȴ����ź�������
	present.pWaitSemaphores = NULL;//�ȴ����ź����б�
	present.pResults = NULL;//���ֲ��������־�б�
}

void MyVulkanManager::initMatrix()
{
	MatrixState3D::setCamera(0, 0, 200, 0, 0, 0, 0, 1, 0);//��ʼ�������
	MatrixState3D::setInitStack();//��ʼ�������任����
	float ratio = (float)screenWidth / (float)screenHeight;//����Ļ�����
	MatrixState3D::setProjectFrustum(-ratio, ratio, -1, 1, 1.5f, 1000);//����ͶӰ����
}

void MyVulkanManager::flushUniformBuffer()//����ǰ֡�����������һ�±�������
{
	xAngle = xAngle + 1.0f;//�ı���ɫ��������ת��
	if (xAngle >= 360) { xAngle = 0; }//������ɫ��������ת�Ƿ�Χ
	MatrixState3D::pushMatrix();//�����ֳ�
	MatrixState3D::rotate(xAngle, 1, 0, 0);//��ת�任
	float* vertexUniformData = MatrixState3D::getFinalMatrix();//��ȡ���ձ任����
	MatrixState3D::popMatrix();//�ָ��ֳ�
	uint8_t *pData;//CPU����ʱ�ĸ���ָ��
	VkResult result = vkMapMemory(device, sqsCL->memUniformBuf, 0, sqsCL->bufferByteCount, 0, (void **)&pData);//���豸�ڴ�ӳ��ΪCPU�ɷ���
	assert(result == VK_SUCCESS);
	memcpy(pData, vertexUniformData, sqsCL->bufferByteCount);//�����վ������ݿ������Դ�
	vkUnmapMemory(device, sqsCL->memUniformBuf);	//����ڴ�ӳ��
}

void MyVulkanManager::flushTexToDesSet()//���»������������ķ���
{
	sqsCL->writes[0].dstSet = sqsCL->descSet[0];//������������Ӧ��д������
	vkUpdateDescriptorSets(device, 1, sqsCL->writes, 0, NULL);//����������
}

void MyVulkanManager::drawObject()
{
	FPSUtil::init();//��ʼ��FPS����
	while (MyVulkanManager::loopDrawFlag)//ÿѭ��һ�λ���һ֡����
	{
		FPSUtil::calFPS();//����FPS
		FPSUtil::before();//һ֡��ʼ

		//��ȡ�������еĵ�ǰ֡����
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentBuffer);
		//Ϊ��Ⱦͨ�����õ�ǰ֡����
		rp_begin.framebuffer = framebuffers[currentBuffer];

		vkResetCommandBuffer(cmdBuffer, 0);//�ָ�����嵽��ʼ״̬
		result = vkBeginCommandBuffer(cmdBuffer, &cmd_buf_info);//���������

		MyVulkanManager::flushUniformBuffer();//����ǰ֡�����������һ�±�������
		MyVulkanManager::flushTexToDesSet();//���»�����������

		vkCmdBeginRenderPass(cmdBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);//������Ⱦͨ��
		triForDraw->drawSelf(cmdBuffer, sqsCL->pipelineLayout, sqsCL->pipeline, &(sqsCL->descSet[0]));//������ɫ������
		vkCmdEndRenderPass(cmdBuffer);//������Ⱦͨ��

		result = vkEndCommandBuffer(cmdBuffer);//���������

		submit_info[0].waitSemaphoreCount = 1;//�ȴ����ź�������
		submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;//�ȴ����ź����б�

		result = vkQueueSubmit(queueGraphics, 1, submit_info, taskFinishFence);//�ύ�����
		do {	//�ȴ���Ⱦ���
			result =vkWaitForFences(device, 1, &taskFinishFence, VK_TRUE, FENCE_TIMEOUT);
		} while (result == VK_TIMEOUT);
		vkResetFences(device, 1, &taskFinishFence);//����դ��
		
		present.pImageIndices = &currentBuffer;//ָ���˴γ��ֵĽ�����ͼ������
		result =vkQueuePresentKHR(queueGraphics, &present);//ִ�г���
		FPSUtil::after(60);//����FPS������ָ����ֵ
	}
}

//ִ�л���
void MyVulkanManager::doVulkan()
{
	ThreadTask* tt = new ThreadTask();
	thread t1(&ThreadTask::doTask, tt);
	t1.detach();
}

void MyVulkanManager::initPipeline()
{
	sqsCL = new ShaderQueueSuit_Common(&device, renderPass, memoryroperties);//������װ����Ⱦ������صĶ���
}

void MyVulkanManager::destroyPipeline()//���ٹ���
{
	delete sqsCL;
}