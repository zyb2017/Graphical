#include "ThreadTask.h"
#include "MyVulkanManager.h"
#include "ShaderQueueSuit_Common.h"
#include "..\legencyUtil\util.hpp"

void ThreadTask::doTask()
{
	MyVulkanManager::init_vulkan_instance();//����Vulkanʵ��
	MyVulkanManager::enumerate_vulkan_phy_devices();//��ȡ�����豸�б�
	MyVulkanManager::create_vulkan_devices();//�����߼��豸
	MyVulkanManager::create_vulkan_CommandBuffer();//���������
	MyVulkanManager::init_queue();//��ȡ�豸��֧��ͼ�ι����Ķ���
	MyVulkanManager::create_vulkan_swapChain();//��ʼ��������
	MyVulkanManager::create_vulkan_DepthBuffer();//������Ȼ���
	MyVulkanManager::create_render_pass();//������Ⱦͨ��
	MyVulkanManager::create_frame_buffer();//����֡����
	MyVulkanManager::createDrawableObject();//���������õ�����
	MyVulkanManager::initPipeline();//��ʼ����Ⱦ����
	MyVulkanManager::createFence();//����դ��
	MyVulkanManager::initPresentInfo();//��ʼ��������Ϣ
	MyVulkanManager::initMatrix();//��ʼ�������任�������������ͶӰ����
	MyVulkanManager::drawObject();//ִ�л���
	MyVulkanManager::destroyFence();//����դ��
	MyVulkanManager::destroyPipeline();//���ٹ���
	MyVulkanManager::destroyDrawableObject();//���ٻ���������
	MyVulkanManager::destroy_frame_buffer();//����֡����
	MyVulkanManager::destroy_render_pass();//������Ⱦͨ�����
	MyVulkanManager::destroy_vulkan_DepthBuffer();//������Ȼ������
	MyVulkanManager::destroy_vulkan_swapChain();//���ٽ��������
	MyVulkanManager::destroy_vulkan_CommandBuffer();//���������
	MyVulkanManager::destroy_vulkan_devices();//�����߼��豸
	MyVulkanManager::destroy_vulkan_instance();//����Vulkan ʵ��
}

ThreadTask::ThreadTask()
{

}
ThreadTask:: ~ThreadTask()
{

}