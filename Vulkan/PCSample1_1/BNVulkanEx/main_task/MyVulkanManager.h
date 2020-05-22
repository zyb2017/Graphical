#ifndef VULKANEXBASE_MYVULKANMANAGER_H
#define VULKANEXBASE_MYVULKANMANAGER_H

#include <vector>
#include <vulkan/vulkan.h>
#include <vector>
#include "../util/DrawableObjectCommon.h"
#include "ShaderQueueSuit_Common.h"

#define FENCE_TIMEOUT 100000000

class MyVulkanManager
{
public:
	//���ڸ����ṹ��
	static struct WindowInfo info;
	//vulkan���Ƶ�ѭ����־
	static bool loopDrawFlag;
	static std::vector<const char *> instanceExtensionNames;//��Ҫʹ�õ�ʵ����չ�����б�
	static VkInstance instance;//Vulkanʵ��
	static uint32_t gpuCount;//�����豸����
	static  std::vector<VkPhysicalDevice> gpus;	//�����豸�б�
	static uint32_t queueFamilyCount;//�����豸��Ӧ�Ķ��м�������
	static std::vector<VkQueueFamilyProperties> queueFamilyprops;//�����豸��Ӧ�Ķ��м��������б�
	static uint32_t queueGraphicsFamilyIndex;//֧��ͼ�ι����Ķ��м�������
	static VkQueue queueGraphics;//֧��ͼ�ι����Ķ���
	static uint32_t queuePresentFamilyIndex;//֧����ʾ�����Ķ��м�������
	static std::vector<const char *> deviceExtensionNames;	//������豸��չ�����б�
	static VkDevice device;	//�߼��豸
	static VkCommandPool cmdPool;//�����
	static VkCommandBuffer cmdBuffer;//�����
	static VkCommandBufferBeginInfo cmd_buf_info;//�����������Ϣ
	static VkCommandBuffer cmd_bufs[1];	//���ύִ�е����������
	static VkSubmitInfo submit_info[1];	//������ύִ����Ϣ����
	static uint32_t screenWidth;//��Ļ���
	static uint32_t screenHeight;//��Ļ�߶�
	static VkSurfaceKHR surface;//KHR����
	static std::vector<VkFormat> formats;//KHR����֧�ֵĸ�ʽ
	static VkSurfaceCapabilitiesKHR surfCapabilities;//���������
	static uint32_t presentModeCount;//��ʾģʽ����
	static std::vector<VkPresentModeKHR> presentModes;//��ʾģʽ�б�
	static VkExtent2D swapchainExtent;//�������ߴ�
	static VkSwapchainKHR swapChain;//������
	static uint32_t swapchainImageCount;//�������е�ͼ������
	static std::vector<VkImage> swapchainImages;//�������е�ͼ���б�
	static std::vector<VkImageView> swapchainImageViews;//��������Ӧ�ĵ�ͼ����ͼ�б�
	static VkFormat depthFormat;//���ͼ���ʽ
	static VkFormatProperties depthFormatProps;	//�����豸֧�ֵ���ȸ�ʽ����
	static VkImage depthImage;//��Ȼ���ͼ��
	static VkPhysicalDeviceMemoryProperties memoryroperties;//�����豸�ڴ�����
	static VkDeviceMemory memDepth;	//��Ȼ���ͼ���Ӧ���ڴ�
	static VkImageView depthImageView;//��Ȼ���ͼ����ͼ
	static VkSemaphore imageAcquiredSemaphore;//��ȾĿ��ͼ���ȡ����ź���
	static uint32_t currentBuffer;//�ӽ������л�ȡ�ĵ�ǰ��Ⱦ��ͼ���Ӧ�Ļ�����
	static VkRenderPass renderPass;//��Ⱦͨ��
	static VkClearValue clear_values[2];//��Ⱦͨ�������֡������ȡ���ɫ����������
	static VkRenderPassBeginInfo rp_begin;//��Ⱦͨ��������Ϣ
	static VkFence taskFinishFence;//�ȴ�������ϵ�դ��
	static VkPresentInfoKHR present;//������Ϣ
	static VkFramebuffer* framebuffers;//֡����������ָ��
	static ShaderQueueSuit_Common* sqsCL;//��ɫ������ָ��
	static DrawableObjectCommonLight* triForDraw;//��������ɫ�������������ָ��
	//��������ת�Ƕ�
	static float xAngle;
	static float yAngle;
	static float zAngle;


	static void init_vulkan_instance();//����Vulkanʵ��
	static void enumerate_vulkan_phy_devices();//��ʼ�������豸
	static void create_vulkan_devices();//�����߼��豸
	static void create_vulkan_CommandBuffer();//���������
	static void create_vulkan_swapChain();//��ʼ��������
	static void create_vulkan_DepthBuffer();//������Ȼ������
	static void create_render_pass();//������Ⱦͨ��
	static void init_queue();//��ȡ�豸��֧��ͼ�ι����Ķ���
	static void create_frame_buffer();//����֡����
	static void createDrawableObject();//��������������
	static void drawObject();//ִ�г����е��������
	static void doVulkan();//�����߳�ִ��Vulkan����
	static void initPipeline();//��ʼ������
	static void createFence();//����դ��
	static void initPresentInfo();//��ʼ����ʾ��Ϣ
	static void initMatrix();//��ʼ������
	static void flushUniformBuffer();//��һ�±����������뻺��
	static void flushTexToDesSet();//�����������������������
	static void destroyFence();//����դ��
	static void destroyPipeline();//���ٹ���
	static void destroyDrawableObject();//���ٻ���������
	static void destroy_frame_buffer();//����֡����
	static void destroy_render_pass();//������Ⱦͨ��
	static void destroy_vulkan_DepthBuffer();//������Ȼ������
	static void destroy_vulkan_swapChain();//���ٽ�����
	static void destroy_vulkan_CommandBuffer();//���������
	static void destroy_vulkan_devices();//�����߼��豸
	static void destroy_vulkan_instance();//����ʵ��
};


#endif //VULKANEXBASE_MYVULKANMANAGER_H
