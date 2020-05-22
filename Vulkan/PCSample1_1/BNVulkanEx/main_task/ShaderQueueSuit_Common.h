#ifndef VULKANEXBASE_SHADERQUEUESUIT_CommonLight_H
#define VULKANEXBASE_SHADERQUEUESUIT_CommonLight_H

#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_Common
{
private:
	VkBuffer uniformBuf;//һ�±�������
	VkDescriptorBufferInfo uniformBufferInfo;//һ�±�������������Ϣ
	std::vector<VkDescriptorSetLayout> descLayouts;//�����������б�
	VkPipelineShaderStageCreateInfo shaderStages[2];//��ɫ���׶�����
	VkVertexInputBindingDescription vertexBinding;//���ߵĶ����������ݰ�����
	VkVertexInputAttributeDescription vertexAttribs[2];//���ߵĶ���������������
	VkPipelineCache pipelineCache;//���߻���
	VkDevice* devicePointer;//�߼��豸ָ��
	VkDescriptorPool descPool;//������
	void create_uniform_buffer(VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties);//����һ�±�������
	void destroy_uniform_buffer(VkDevice& device);//����һ�±�������
	void create_pipeline_layout(VkDevice& device);//�������߲���
	void destroy_pipeline_layout(VkDevice& device);	//���ٹ��߲���
	void init_descriptor_set(VkDevice& device);	//��ʼ��������
	void create_shader(VkDevice& device);//������ɫ��
	void destroy_shader(VkDevice& device);//������ɫ��
	void initVertexAttributeInfo();//��ʼ����������������Ϣ
	void create_pipe_line(VkDevice& device, VkRenderPass& renderPass);//��������
	void destroy_pipe_line(VkDevice& device);//���ٹ���
public:
	int bufferByteCount;//һ�»������ֽ���
	VkDeviceMemory memUniformBuf;//һ�±��������ڴ�
	VkWriteDescriptorSet writes[1];//һ�±���д��������
	std::vector<VkDescriptorSet> descSet;//�������б�
	VkPipelineLayout pipelineLayout;//���߲���
	VkPipeline pipeline;//����
	ShaderQueueSuit_Common(VkDevice* deviceIn, VkRenderPass&renderPass, VkPhysicalDeviceMemoryProperties& memoryroperties);//���캯��
	~ShaderQueueSuit_Common();//��������
};

#endif
