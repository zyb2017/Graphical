#ifndef VULKANEXBASE_SHADERQUEUESUIT_CommonLight_H
#define VULKANEXBASE_SHADERQUEUESUIT_CommonLight_H

#include <vector>
#include <vulkan/vulkan.h>


class ShaderQueueSuit_Common
{
private:
	VkBuffer uniformBuf;//一致变量缓冲
	VkDescriptorBufferInfo uniformBufferInfo;//一致变量缓冲描述信息
	std::vector<VkDescriptorSetLayout> descLayouts;//描述集布局列表
	VkPipelineShaderStageCreateInfo shaderStages[2];//着色器阶段数组
	VkVertexInputBindingDescription vertexBinding;//管线的顶点输入数据绑定描述
	VkVertexInputAttributeDescription vertexAttribs[2];//管线的顶点输入属性描述
	VkPipelineCache pipelineCache;//管线缓冲
	VkDevice* devicePointer;//逻辑设备指针
	VkDescriptorPool descPool;//描述池
	void create_uniform_buffer(VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties);//创建一致变量缓冲
	void destroy_uniform_buffer(VkDevice& device);//销毁一致变量缓冲
	void create_pipeline_layout(VkDevice& device);//创建管线布局
	void destroy_pipeline_layout(VkDevice& device);	//销毁管线布局
	void init_descriptor_set(VkDevice& device);	//初始化描述集
	void create_shader(VkDevice& device);//创建着色器
	void destroy_shader(VkDevice& device);//销毁着色器
	void initVertexAttributeInfo();//初始化顶点输入属性信息
	void create_pipe_line(VkDevice& device, VkRenderPass& renderPass);//创建管线
	void destroy_pipe_line(VkDevice& device);//销毁管线
public:
	int bufferByteCount;//一致缓冲总字节数
	VkDeviceMemory memUniformBuf;//一致变量缓冲内存
	VkWriteDescriptorSet writes[1];//一致变量写入描述集
	std::vector<VkDescriptorSet> descSet;//描述集列表
	VkPipelineLayout pipelineLayout;//管线布局
	VkPipeline pipeline;//管线
	ShaderQueueSuit_Common(VkDevice* deviceIn, VkRenderPass&renderPass, VkPhysicalDeviceMemoryProperties& memoryroperties);//构造函数
	~ShaderQueueSuit_Common();//析构函数
};

#endif
