#ifndef VULKANEXBASE_OBJOBJECT_H
#define VULKANEXBASE_OBJOBJECT_H

#include <vulkan/vulkan.h>
#include <string>

class DrawableObjectCommonLight
{
public:
	VkDevice* devicePointer;//ָ���߼��豸��ָ��
	float* vdata;//�������������׵�ַָ��
	int vCount;//��������
	VkBuffer vertexDatabuf;//�������ݻ���
	VkDeviceMemory vertexDataMem;//�������������豸�ڴ�
	VkDescriptorBufferInfo vertexDataBufferInfo;//�������ݻ���������Ϣ
	DrawableObjectCommonLight(float* vdataIn, int dataByteCount, int vCountIn, VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties);//���캯��
	~DrawableObjectCommonLight();//��������
	void drawSelf(VkCommandBuffer& secondary_cmd, VkPipelineLayout& pipelineLayout, VkPipeline& pipeline, VkDescriptorSet* desSetPointer);//���Ʒ���
};


#endif
