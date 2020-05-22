#include "HelpFunction.h"

//�ڲ�ʹ�õ�ȷ���ڴ����������ķ���
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties& memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex)
{
	//ѭ��ȷ���ڴ���������
	for (uint32_t i = 0; i < 32; i++)
	{
		//����Ӧ���ͱ���λΪ1
		if ((typeBits & 1) == 1)
		{
			//������������ƥ��
			if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	//û���ҵ��������������
	return false;
}

