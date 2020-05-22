#ifndef VULKANEXBASE_SHADERCOMPILEUTIL_H
#define VULKANEXBASE_SHADERCOMPILEUTIL_H

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

//��GLSL��ɫ���ű�����Ϊspirv�ķ���
bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv);

#endif //VULKANEXBASE_SHADERCOMPILEUTIL_H
