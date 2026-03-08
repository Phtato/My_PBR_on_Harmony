//
// Created by 新垣つつ on 2026/3/6.
//

#include "render/include/ShaderLibrary.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "VulkanConfig.h"
#include "render/include/common.h"

std::vector<uint32_t> ShaderLibrary::LoadSPIRV(const std::string& filename)
{
    // 检查缓存
    auto it = m_cache.find(filename);
    if (it != m_cache.end()) {
        return it->second;
    }

    // 优先尝试鸿蒙 rawfile 加载
    std::vector<uint32_t> code;

    auto filePath = VulkanConfig::getInstance().getOhosPath() + "/data/shaders/" + filename;
    std::ifstream is(filePath, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        
        if (size % 4 != 0) {
            std::cerr << "Error: SPIR-V file size must be a multiple of 4: \"" << filename << "\"" << std::endl;
        } else {
            code.resize(size / 4);
            is.read(reinterpret_cast<char*>(code.data()), size);
        }
        is.close();
    }
    else {
        std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
    }

    // 缓存结果
    m_cache[filename] = code;
    return code;
}

ShaderLibrary::ShaderPair ShaderLibrary::LoadShaderPair(const std::string& baseName)
{
    // TODO: 加载 .vert 和 .frag 对应的 SPIR-V 文件
    ShaderPair pair;
    pair.vertex = LoadSPIRV(baseName + ".vert.spv");
    pair.fragment = LoadSPIRV(baseName + ".frag.spv");
    return pair;
}

std::vector<uint32_t> ShaderLibrary::LoadSPIRVFromPath(const std::string& filePath)
{
    return ReadBinaryFile(filePath);
}

std::vector<uint32_t> ShaderLibrary::ReadBinaryFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filePath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    
    if (fileSize % 4 != 0) {
        throw std::runtime_error("SPIR-V file size must be a multiple of 4: " + filePath);
    }

    std::vector<uint32_t> buffer(fileSize / 4);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

void ShaderLibrary::ClearCache()
{
    m_cache.clear();
}
