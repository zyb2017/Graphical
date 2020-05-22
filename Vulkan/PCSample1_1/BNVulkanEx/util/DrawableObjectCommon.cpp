#include "DrawableObjectCommon.h"
#include <assert.h>
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include <string.h>

DrawableObjectCommonLight::DrawableObjectCommonLight(float* vdataIn, int dataByteCount, int vCountIn, VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties)
{
	this->devicePointer = &device;
	this->vdata = vdataIn;
	this->vCount = vCountIn;

	VkBufferCreateInfo buf_info = {};//�������崴����Ϣ�ṹ��ʵ��
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;//���ýṹ������
	buf_info.pNext = NULL;//�Զ������ݵ�ָ��
	buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;//�������;Ϊ��������
	buf_info.size = dataByteCount;//�����������ֽ���
	buf_info.queueFamilyIndexCount = 0;//���м�������
	buf_info.pQueueFamilyIndices = NULL;//���м��������б�
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//����ģʽ
	buf_info.flags = 0;//��־

	VkResult result = vkCreateBuffer(device, &buf_info, NULL, &vertexDatabuf);//��������
	assert(result == VK_SUCCESS);//��黺�崴���Ƿ�ɹ�

	VkMemoryRequirements mem_reqs;//�����ڴ�����
	vkGetBufferMemoryRequirements(device, vertexDatabuf, &mem_reqs);//��ȡ�����ڴ�����
	assert(dataByteCount <= mem_reqs.size);//����ڴ������ȡ�Ƿ���ȷ

	VkMemoryAllocateInfo alloc_info = {};//�����ڴ������Ϣ�ṹ��ʵ��
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//�ṹ������
	alloc_info.pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info.memoryTypeIndex = 0;//�ڴ���������
	alloc_info.allocationSize = mem_reqs.size;//�ڴ����ֽ���

	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//��Ҫ���ڴ���������
	bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);//��ȡ�����ڴ���������
	if (flag)
	{
		printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d", alloc_info.memoryTypeIndex);
	}
	else
	{
		printf("ȷ���ڴ�����ʧ��!");
	}

	result = vkAllocateMemory(device, &alloc_info, NULL, &vertexDataMem);//Ϊ�������ݻ�������ڴ�
	assert(result == VK_SUCCESS);

	uint8_t *pData;//CPU����ʱ�ĸ���ָ��
	result = vkMapMemory(device, vertexDataMem, 0, mem_reqs.size, 0, (void **)&pData);//���豸�ڴ�ӳ��ΪCPU�ɷ���
	assert(result == VK_SUCCESS);//���ӳ���Ƿ�ɹ�
	memcpy(pData, vdata, dataByteCount);//���������ݿ������豸�ڴ�
	vkUnmapMemory(device, vertexDataMem);//����ڴ�ӳ��
	result = vkBindBufferMemory(device, vertexDatabuf, vertexDataMem, 0);//���ڴ��뻺��
	assert(result == VK_SUCCESS);

	vertexDataBufferInfo.buffer = vertexDatabuf;//ָ�����ݻ���
	vertexDataBufferInfo.offset = 0;//���ݻ�����ʼƫ����
	vertexDataBufferInfo.range = mem_reqs.size;//���ݻ�����ռ�ֽ���
}

DrawableObjectCommonLight::~DrawableObjectCommonLight()
{
	delete vdata;//�ͷ�ָ���ڴ�
	vkDestroyBuffer(*devicePointer, vertexDatabuf, NULL);//���ٶ��㻺��
	vkFreeMemory(*devicePointer, vertexDataMem, NULL);//�ͷ��豸�ڴ�
}

void DrawableObjectCommonLight::drawSelf(VkCommandBuffer& cmd, VkPipelineLayout& pipelineLayout, VkPipeline& pipeline, VkDescriptorSet* desSetPointer)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);//����ǰʹ�õ��������ָ�����߰�
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, NULL);//������塢���߲��֡���������
	const VkDeviceSize offsetsVertex[1] = { 0 };//��������ƫ��������
	vkCmdBindVertexBuffers(//�����������뵱ǰʹ�õ�������
		cmd,				//��ǰʹ�õ������
		0,					//�������ݻ������б��е�������
		1,					//�󶨶��㻺�������
		&(vertexDatabuf),	//�󶨵Ķ������ݻ����б�
		offsetsVertex		//�����������ݻ�����ڲ�ƫ����
	);
	vkCmdDraw(cmd, vCount, 1, 0, 0);//ִ�л���
}