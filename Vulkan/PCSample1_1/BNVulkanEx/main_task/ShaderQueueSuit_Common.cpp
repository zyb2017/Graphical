#include "ShaderQueueSuit_Common.h"
#include <assert.h>
#include "../util/HelpFunction.h"
#include "../util/FileUtil.h"
#include "MyVulkanManager.h"
#include "ShaderCompileUtil.h"
#include "../legencyUtil/util.hpp"
#include "PathData.h"

//����Unifirm Buffer
void ShaderQueueSuit_Common::create_uniform_buffer(VkDevice& device, VkPhysicalDeviceMemoryProperties& memoryroperties)
{
	bufferByteCount = sizeof(float) * 16;//һ�±�����������ֽ���

	VkBufferCreateInfo buf_info = {};//����һ�±������崴����Ϣ�ṹ��ʵ��
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;	//�ṹ�������
	buf_info.pNext = NULL;//�Զ������ݵ�ָ��
	buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;//�������;
	buf_info.size = bufferByteCount;//�������ֽ���
	buf_info.queueFamilyIndexCount = 0;	//���м�������
	buf_info.pQueueFamilyIndices = NULL;//���м��������б�
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//����ģʽ
	buf_info.flags = 0;//��־

	VkResult result = vkCreateBuffer(device, &buf_info, NULL, &uniformBuf);//����һ�±�������
	assert(result == VK_SUCCESS);//��鴴���Ƿ�ɹ�

	VkMemoryRequirements mem_reqs;//�ڴ��������
	vkGetBufferMemoryRequirements(device, uniformBuf, &mem_reqs);//��ȡ�˻�����ڴ�����

	VkMemoryAllocateInfo alloc_info = {};//�����ڴ������Ϣ�ṹ��ʵ��
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//�ṹ������
	alloc_info.pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info.memoryTypeIndex = 0;//�ڴ���������
	alloc_info.allocationSize = mem_reqs.size;//�����ڴ�����ֽ���

	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//��Ҫ���ڴ���������
	bool flag = memoryTypeFromProperties(memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);	//��ȡ�����ڴ���������
	if (flag) { printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d", alloc_info.memoryTypeIndex); }
	else { printf("ȷ���ڴ�����ʧ��!"); }

	result = vkAllocateMemory(device, &alloc_info, NULL, &memUniformBuf);//�����ڴ�
	assert(result == VK_SUCCESS);//����ڴ�����Ƿ�ɹ�
	result = vkBindBufferMemory(device, uniformBuf, memUniformBuf, 0);//���ڴ�Ͷ�Ӧ�����
	assert(result == VK_SUCCESS);//���󶨲����Ƿ�ɹ�

	uniformBufferInfo.buffer = uniformBuf;//ָ��һ�±�������
	uniformBufferInfo.offset = 0;//��ʼƫ����
	uniformBufferInfo.range = bufferByteCount;//һ�±����������ֽ���
}

void ShaderQueueSuit_Common::destroy_uniform_buffer(VkDevice& device)//����һ�±����������
{
	vkDestroyBuffer(device, uniformBuf, NULL);//����һ�±�������
	vkFreeMemory(device, memUniformBuf, NULL);//�ͷ�һ�±��������Ӧ�豸�ڴ�
}

//��������layout
void ShaderQueueSuit_Common::create_pipeline_layout(VkDevice& device)
{

	VkDescriptorSetLayoutBinding layout_bindings[1];//���������ְ�����
	layout_bindings[0].binding = 0;//�˰󶨵İ󶨵���
	layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	layout_bindings[0].descriptorCount = 1;//��������
	layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;	//Ŀ����ɫ���׶�
	layout_bindings[0].pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo descriptor_layout = {};	//�������������ִ�����Ϣ�ṹ��ʵ��
	descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//�ṹ������
	descriptor_layout.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_layout.bindingCount = 1;//���������ְ󶨵�����
	descriptor_layout.pBindings = layout_bindings;//���������ְ�����

	//�����ռ�
	descLayouts.resize(NUM_DESCRIPTOR_SETS);//���������������б�ߴ�
	VkResult result = vkCreateDescriptorSetLayout(device, &descriptor_layout, NULL, descLayouts.data());//��������������
	assert(result == VK_SUCCESS);//������������ִ����Ƿ�ɹ�

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};//�������߲��ִ�����Ϣ�ṹ��ʵ��
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;	//�ṹ������
	pPipelineLayoutCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;//���ͳ�����Χ������
	pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;//���ͳ�����Χ���б�
	pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;//���������ֵ�����
	pPipelineLayoutCreateInfo.pSetLayouts = descLayouts.data();//�����������б�

	result = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, NULL, &pipelineLayout);//�������߲���
	assert(result == VK_SUCCESS);//��鴴���Ƿ�ɹ�
}

void ShaderQueueSuit_Common::destroy_pipeline_layout(VkDevice& device)//���ٹ��߲��ֵķ���
{
	for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)//�����������б�
	{
		vkDestroyDescriptorSetLayout(device, descLayouts[i], NULL);//���ٶ�Ӧ����������
	}
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);//���ٹ��߲���
}

void ShaderQueueSuit_Common::init_descriptor_set(VkDevice& device)//��ʼ��������
{
	VkDescriptorPoolSize type_count[1];//�������سߴ�ʵ������
	type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	type_count[0].descriptorCount = 1;//��������

	VkDescriptorPoolCreateInfo descriptor_pool = {};//�����������ش�����Ϣ�ṹ��ʵ��
	descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//�ṹ������
	descriptor_pool.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_pool.maxSets = 1;//�������������
	descriptor_pool.poolSizeCount = 1;//�������سߴ�ʵ������
	descriptor_pool.pPoolSizes = type_count;//�������سߴ�ʵ������

	VkResult result = vkCreateDescriptorPool(device, &descriptor_pool, NULL, &descPool);//������������
	assert(result == VK_SUCCESS);//����������ش����Ƿ�ɹ�

	std::vector<VkDescriptorSetLayout> layouts;//�����������б�
	layouts.push_back(descLayouts[0]);//���б������ָ������������

	VkDescriptorSetAllocateInfo alloc_info[1];//����������������Ϣ�ṹ��ʵ������
	alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//�ṹ������
	alloc_info[0].pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info[0].descriptorPool = descPool;//ָ����������
	alloc_info[0].descriptorSetCount = 1;//����������
	alloc_info[0].pSetLayouts = layouts.data();//�����������б�

	descSet.resize(1);//�����������б�ߴ�
	result = vkAllocateDescriptorSets(device, alloc_info, descSet.data());//����������
	assert(result == VK_SUCCESS);//��������������Ƿ�ɹ�

	writes[0] = {}; //����һ�±���д��������ʵ������
	writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//�ṹ������
	writes[0].pNext = NULL;	//�Զ������ݵ�ָ��
	writes[0].descriptorCount = 1;//��������
	writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	writes[0].pBufferInfo = &uniformBufferInfo;//��Ӧһ�±����������Ϣ
	writes[0].dstArrayElement = 0;//Ŀ��������ʼԪ��
	writes[0].dstBinding = 0;//Ŀ��󶨱��
}

void ShaderQueueSuit_Common::create_shader(VkDevice& device)
{
	char * ss = VertShaderPath;
	char * vs = FragShaderPath;

	init_glslang();//Windows��ʼ����Ⱦ���̱���

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].pNext = NULL;//�Զ������ݵ�ָ��
	shaderStages[0].pSpecializationInfo = NULL;//������Ϣ
	shaderStages[0].flags = 0;//������ʹ�õı�־
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;	//��ɫ���׶�Ϊ����
	shaderStages[0].pName = "main";//��ں���Ϊmain


	std::vector<unsigned int> vtx_spv;//��������ɫ���ű�����ΪSPV
	bool retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, FileUtil::loadAssetStr(ss).c_str(), vtx_spv);
	assert(retVal);//�������Ƿ�ɹ�
	printf("������ɫ���ű�����SPV�ɹ���\n");

	VkShaderModuleCreateInfo moduleCreateInfo;//׼��������ɫ��ģ�鴴����Ϣ
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	moduleCreateInfo.flags = 0;//������ʹ�õı�־
	moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);//������ɫ��SPV�������ֽ���
	moduleCreateInfo.pCode = vtx_spv.data();//������ɫ��SPV����

	VkResult result = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderStages[0].module);//����������ɫ��ģ��
	assert(result == VK_SUCCESS);//��鶥����ɫ��ģ�鴴���Ƿ�ɹ�

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//�ṹ������
	shaderStages[1].pNext = NULL;//�Զ������ݵ�ָ��
	shaderStages[1].pSpecializationInfo = NULL;//������Ϣ
	shaderStages[1].flags = 0;//������ʹ�õı�־
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;//��ɫ���׶�ΪƬԪ
	shaderStages[1].pName = "main";//��ں���Ϊmain

	std::vector<unsigned int> frag_spv;
	retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, FileUtil::loadAssetStr(vs).c_str(), frag_spv);//��ƬԪ��ɫ���ű�����ΪSPV
	assert(retVal);//�������Ƿ�ɹ�
	printf("ƬԪ��ɫ���ű�����SPV�ɹ���\n");
	//׼��ƬԪ��ɫ��ģ�鴴����Ϣ
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;//���ýṹ������
	moduleCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	moduleCreateInfo.flags = 0;//������ʹ�õı�־
	moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);//ƬԪ��ɫ��SPV�������ֽ���
	moduleCreateInfo.pCode = frag_spv.data();//ƬԪ��ɫ��SPV����
	//����ƬԪ��ɫ��ģ��
	result = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderStages[1].module);
	assert(result == VK_SUCCESS);//���ƬԪ��ɫ��ģ�鴴���Ƿ�ɹ�
}

void ShaderQueueSuit_Common::destroy_shader(VkDevice& device)//������ɫ��ģ��ķ���
{
	vkDestroyShaderModule(device, shaderStages[0].module, NULL);//���ٶ�����ɫ��ģ��
	vkDestroyShaderModule(device, shaderStages[1].module, NULL);//����ƬԪ��ɫ��ģ��
}

void ShaderQueueSuit_Common::initVertexAttributeInfo()
{
	vertexBinding.binding = 0;//��Ӧ�󶨵�
	vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;	//��������Ƶ��Ϊÿ����
	vertexBinding.stride = sizeof(float) * 6;//ÿ�����ݵĿ���ֽ���

	vertexAttribs[0].binding = 0;//��1�������������Եİ󶨵�
	vertexAttribs[0].location = 0;//��1�������������Ե�λ������
	vertexAttribs[0].format = VK_FORMAT_R32G32B32_SFLOAT;//��1�������������Ե����ݸ�ʽ
	vertexAttribs[0].offset = 0;//��1�������������Ե�ƫ����

	vertexAttribs[1].binding = 0;//��2�������������Եİ󶨵�
	vertexAttribs[1].location = 1;//��2�������������Ե�λ������
	vertexAttribs[1].format = VK_FORMAT_R32G32B32_SFLOAT;//��2�������������Ե����ݸ�ʽ
	vertexAttribs[1].offset = 12;//��2�������������Ե�ƫ����
}

void ShaderQueueSuit_Common::create_pipe_line(VkDevice& device, VkRenderPass& renderPass)
{
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];//��̬״̬���ñ�־
	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);	//�������б�־Ϊfalse

	VkPipelineDynamicStateCreateInfo dynamicState = {};//���߶�̬״̬������Ϣ
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;//�ṹ������
	dynamicState.pNext = NULL;//�Զ������ݵ�ָ��
	dynamicState.pDynamicStates = dynamicStateEnables;//��̬״̬���ñ�־����
	dynamicState.dynamicStateCount = 0;//���õĶ�̬״̬������

	VkPipelineVertexInputStateCreateInfo vi;//���߶�����������״̬������Ϣ
	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.pNext = NULL;//�Զ������ݵ�ָ��
	vi.flags = 0;//������ʹ�õı�־
	vi.vertexBindingDescriptionCount = 1;//�����������������
	vi.pVertexBindingDescriptions = &vertexBinding;//��������������б�
	vi.vertexAttributeDescriptionCount = 2;//����������������
	vi.pVertexAttributeDescriptions = vertexAttribs;//�����������������б�

	VkPipelineInputAssemblyStateCreateInfo ia;//����ͼԪ��װ״̬������Ϣ
	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.pNext = NULL;//�Զ������ݵ�ָ��
	ia.flags = 0;//������ʹ�õı�־
	ia.primitiveRestartEnable = VK_FALSE;//�ر�ͼԪ����
	ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	//����������ͼԪ�б�ģʽ

	VkPipelineRasterizationStateCreateInfo rs;//���߹�դ��״̬������Ϣ
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = NULL;//�Զ������ݵ�ָ��
	rs.flags = 0;//������ʹ�õı�־
	rs.polygonMode = VK_POLYGON_MODE_FILL;//���Ʒ�ʽΪ���
	rs.cullMode = VK_CULL_MODE_NONE;//��ʹ�ñ������
	rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;	//���Ʒ���Ϊ��ʱ��
	rs.depthClampEnable = VK_TRUE;//��Ƚ�ȡ
	rs.rasterizerDiscardEnable = VK_FALSE;//���ù�դ����������ΪTRUE���դ���������κ�ƬԪ��
	rs.depthBiasEnable = VK_FALSE;//���������ƫ��
	rs.depthBiasConstantFactor = 0;	//���ƫ�Ƴ�������
	rs.depthBiasClamp = 0;//���ƫ��ֵ�����ޣ���Ϊ����Ϊ���ޣ�Ϊ����Ϊ���ޣ�
	rs.depthBiasSlopeFactor = 0;//���ƫ��б������
	rs.lineWidth = 1.0f;//�߿�ȣ������߻���ģʽ�����ã�

	VkPipelineColorBlendAttachmentState att_state[1];//������ɫ��ϸ���״̬����
	att_state[0].colorWriteMask = 0xf;//����д������
	att_state[0].blendEnable = VK_FALSE;//�رջ��
	att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;//����Alphaͨ����Ϸ�ʽ
	att_state[0].colorBlendOp = VK_BLEND_OP_ADD;//����RGBͨ����Ϸ�ʽ
	att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;//����Դ��ɫ�������
	att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;//����Ŀ����ɫ�������
	att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;//����ԴAlpha�������
	att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;//����Ŀ��Alpha�������


	VkPipelineColorBlendStateCreateInfo cb;//���ߵ���ɫ���״̬������Ϣ
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.pNext = NULL;//�Զ������ݵ�ָ��
	cb.flags = 0;//��δ��ʹ�õı�־
	cb.attachmentCount = 1;	//��ɫ��ϸ�������
	cb.pAttachments = att_state;//��ɫ��ϸ����б�
	cb.logicOpEnable = VK_FALSE;//�������߼�����
	cb.logicOp = VK_LOGIC_OP_NO_OP;//�߼���������Ϊ��
	cb.blendConstants[0] = 1.0f;//��ϳ���R����
	cb.blendConstants[1] = 1.0f;//��ϳ���G����
	cb.blendConstants[2] = 1.0f;//��ϳ���B����
	cb.blendConstants[3] = 1.0f;//��ϳ���A����

	VkViewport viewports;//�ӿ���Ϣ
	viewports.minDepth = 0.0f;//�ӿ���С���
	viewports.maxDepth = 1.0f;//�ӿ�������
	viewports.x = 0;//�ӿ�X����
	viewports.y = 0;//�ӿ�Y����
	viewports.width = MyVulkanManager::screenWidth;//�ӿڿ��
	viewports.height = MyVulkanManager::screenHeight;//�ӿڸ߶�

	VkRect2D scissor;//���ô�����Ϣ
	scissor.extent.width = MyVulkanManager::screenWidth;//���ô��ڵĿ��
	scissor.extent.height = MyVulkanManager::screenHeight;//���ô��ڵĸ߶�
	scissor.offset.x = 0;//���ô��ڵ�X����
	scissor.offset.y = 0;//���ô��ڵ�Y����

	VkPipelineViewportStateCreateInfo vp = {};//�����ӿ�״̬������Ϣ
	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.pNext = NULL;//�Զ������ݵ�ָ��
	vp.flags = 0;//������ʹ�õı�־
	vp.viewportCount = 1;//�ӿڵ�����
	vp.scissorCount = 1;//���ô��ڵ�����
	vp.pScissors = &scissor;//���ô�����Ϣ�б�
	vp.pViewports = &viewports;//�ӿ���Ϣ�б�

	VkPipelineDepthStencilStateCreateInfo ds;//������ȼ�ģ��״̬������Ϣ
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.pNext = NULL;//�Զ������ݵ�ָ��
	ds.flags = 0;//������ʹ�õı�־
	ds.depthTestEnable = VK_TRUE;//������Ȳ���
	ds.depthWriteEnable = VK_TRUE;//�������ֵд��
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;//��ȼ��Ƚϲ���
	ds.depthBoundsTestEnable = VK_FALSE;//�ر���ȱ߽����
	ds.minDepthBounds = 0;//��С��ȱ߽�
	ds.maxDepthBounds = 0;//�����ȱ߽�
	ds.stencilTestEnable = VK_FALSE;//�ر�ģ�����
	ds.back.failOp = VK_STENCIL_OP_KEEP;//δͨ��ģ�����ʱ�Ĳ���
	ds.back.passOp = VK_STENCIL_OP_KEEP;//ģ����ԡ���Ȳ��Զ�ͨ��ʱ�Ĳ���
	ds.back.compareOp = VK_COMPARE_OP_ALWAYS;//ģ����ԵıȽϲ���
	ds.back.compareMask = 0;//ģ����ԱȽ�����
	ds.back.reference = 0;//ģ����Բο�ֵ
	ds.back.depthFailOp = VK_STENCIL_OP_KEEP;//δͨ����Ȳ���ʱ�Ĳ���
	ds.back.writeMask = 0;//д������
	ds.front = ds.back;

	VkPipelineMultisampleStateCreateInfo ms;//���߶��ز���״̬������Ϣ
	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pNext = NULL;//�Զ������ݵ�ָ��
	ms.flags = 0;//������ʹ�õı�־λ
	ms.pSampleMask = NULL;//��������
	ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//��դ���׶β�������
	ms.sampleShadingEnable = VK_FALSE;//�رղ�����ɫ
	ms.alphaToCoverageEnable = VK_FALSE;//������alphaToCoverage
	ms.alphaToOneEnable = VK_FALSE;//������alphaToOne
	ms.minSampleShading = 0.0;//��С������ɫ

	VkGraphicsPipelineCreateInfo pipelineInfo;//ͼ�ι��ߴ�����Ϣ
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pipelineInfo.layout = pipelineLayout;//ָ�����߲���
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//�����߾��
	pipelineInfo.basePipelineIndex = 0;//����������
	pipelineInfo.flags = 0;	//��־
	pipelineInfo.pVertexInputState = &vi;//���ߵĶ�����������״̬��Ϣ
	pipelineInfo.pInputAssemblyState = &ia;//���ߵ�ͼԪ��װ״̬��Ϣ
	pipelineInfo.pRasterizationState = &rs;//���ߵĹ�դ��״̬��Ϣ
	pipelineInfo.pColorBlendState = &cb;//���ߵ���ɫ���״̬��Ϣ
	pipelineInfo.pTessellationState = NULL;//���ߵ�����ϸ��״̬��Ϣ
	pipelineInfo.pMultisampleState = &ms;//���ߵĶ��ز���״̬��Ϣ
	pipelineInfo.pDynamicState = &dynamicState;//���ߵĶ�̬״̬��Ϣ
	pipelineInfo.pViewportState = &vp;//���ߵ��ӿ�״̬��Ϣ
	pipelineInfo.pDepthStencilState = &ds; //���ߵ����ģ�����״̬��Ϣ
	pipelineInfo.stageCount = 2;//���ߵ���ɫ�׶�����
	pipelineInfo.pStages = shaderStages;//���ߵ���ɫ�׶��б�
	pipelineInfo.renderPass = renderPass;//ָ������Ⱦͨ��
	pipelineInfo.subpass = 0;//���ù���ִ�ж�Ӧ����Ⱦ��ͨ��

	VkPipelineCacheCreateInfo pipelineCacheInfo;//���߻��崴����Ϣ
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pipelineCacheInfo.initialDataSize = 0;//��ʼ���ݳߴ�
	pipelineCacheInfo.pInitialData = NULL;//��ʼ�������ݣ��˴�ΪNULL
	pipelineCacheInfo.flags = 0;//������ʹ�õı�־λ
	
	VkResult result = vkCreatePipelineCache(device, &pipelineCacheInfo, NULL, &pipelineCache);//�������߻���
	assert(result == VK_SUCCESS);//�����߻��崴���Ƿ�ɹ�
	result = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, NULL, &pipeline);//��������
	assert(result == VK_SUCCESS);//�����ߴ����Ƿ�ɹ�
}

void ShaderQueueSuit_Common::destroy_pipe_line(VkDevice& device)
{
	//���ٹ���
	vkDestroyPipeline(device, pipeline, NULL);
	//���ٹ��߻���
	vkDestroyPipelineCache(device, pipelineCache, NULL);
}

ShaderQueueSuit_Common::ShaderQueueSuit_Common(VkDevice* deviceIn, VkRenderPass& renderPass, VkPhysicalDeviceMemoryProperties& memoryroperties)
{
	this->devicePointer = deviceIn;
	create_uniform_buffer(*devicePointer, memoryroperties);//����һ�±�������
	create_pipeline_layout(*devicePointer);//�������߲���
	init_descriptor_set(*devicePointer);//��ʼ��������
	create_shader(*devicePointer);//������ɫ��
	initVertexAttributeInfo();//��ʼ������������Ϣ
	create_pipe_line(*devicePointer, renderPass);//��������
}

ShaderQueueSuit_Common::~ShaderQueueSuit_Common()//��������
{
	destroy_pipe_line(*devicePointer);//���ٹ���
	destroy_shader(*devicePointer);//������ɫ��ģ��
	destroy_pipeline_layout(*devicePointer);//���ٹ��߲���
	destroy_uniform_buffer(*devicePointer);//����һ�±�������
}