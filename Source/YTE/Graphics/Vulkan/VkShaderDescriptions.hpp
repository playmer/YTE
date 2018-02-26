///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkShaderDescriptions_hpp
#define YTE_Graphics_Vulkan_VkShaderDescriptions_hpp

#include "fmt/format.h"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"
#include "YTE/Graphics/Vertex.hpp"

namespace YTE
{
  class VkShaderDescriptions
  {
  public:
    VkShaderDescriptions(size_t aNumberOfBindings = 2, size_t aNumberOfAttributes = 8)
    {
      mBindings.reserve(aNumberOfBindings);
      mAttributes.reserve(aNumberOfAttributes);
    }

    template <typename T>
    void AddAttribute(vk::Format aFormat)
    {
      DebugObjection(mBindings.size() == 0,
        "Haven't added a Vertex binding yet, "
        "so we can't add attribute inputs.");

      vk::VertexInputAttributeDescription toAdd;
      toAdd.binding = mBinding - 1;
      toAdd.location = mLocation;
      toAdd.format = aFormat;
      toAdd.offset = mVertexOffset;

      mAttributes.emplace_back(toAdd);

      ++mLocation;
      mVertexOffset += sizeof(T);
    }

    template <typename T>
    void AddSpecializationEntry(T &aValue)
    {
      vk::SpecializationMapEntry entry;
      entry.constantID = mConstant;
      entry.size = sizeof(T);

      mEntries.emplace_back(entry);
      ++mConstant;
    }

    template <typename T>
    void AddBinding(vk::VertexInputRate aDescription)
    {
      vk::VertexInputBindingDescription toAdd;
      toAdd.binding = mBinding;
      toAdd.inputRate = aDescription;
      toAdd.stride = sizeof(T);

      mBindings.emplace_back(toAdd);

      ++mBinding;
      mVertexOffset = 0;
    }


    template <typename T>
    void AddBuffer(std::string_view aName, 
                   vk::DescriptorType aType, 
                   vk::ShaderStageFlagBits aShader)
    {


      mDescriptorSetLayout.emplace_back(mBufferBinding++, aType, aShader, nullptr);
    }


    /////////////////////////////////
    // Getter 
    /////////////////////////////////
    vk::VertexInputBindingDescription* BindingData()
    {
      return mBindings.data();
    }

    size_t BindingSize()
    {
      return mBindings.size();
    }

    vk::VertexInputAttributeDescription* AttributeData()
    {
      return mAttributes.data();
    }

    size_t AttributeSize()
    {
      return mAttributes.size();
    }

    std::vector<vk::VertexInputBindingDescription>& Bindings()
    {
      return mBindings;
    }

    std::vector<vk::VertexInputAttributeDescription>& Attributes()
    {
      return mAttributes;
    }


    std::unique_ptr<vkhlf::SpecializationInfo> PipelineShaderStageCreateInfo()
    {
      return std::make_unique<vkhlf::SpecializationInfo>(mEntries, mData);
    }

    void AddPreludeLine(std::string_view aLine)
    {
      mLines.emplace_back(aLine.begin(), aLine.end());
    }

    std::string GetLines()
    {
      fmt::MemoryWriter w;

      for (auto &line : mLines)
      {
        w.write(line);
        w.write("\n");
      }

      return w.str();
    }

  private:

    template <typename T>
    void CopyEntryData(T &aValue)
    {
      auto current = mData.size();
      
      mData.resize(current + sizeof(T), 0);

      std::memcpy(mData.data() + current, static_cast<void*>(&aValue), sizeof(T));
    }

    // Vertex Input Data
    std::vector<vk::VertexInputBindingDescription> mBindings;
    std::vector<vk::VertexInputAttributeDescription> mAttributes;
    std::vector<std::string> mLines;
    u32 mBinding = 0;
    u32 mVertexOffset = 0;
    u32 mLocation = 0;
    u32 mConstant = 0;

    // Buffer Data
    std::vector<vkhlf::DescriptorSetLayoutBinding> mDescriptorSetLayout;
    u32 mBufferBinding = 0;


    std::vector<vk::SpecializationMapEntry> mEntries;
    std::vector<byte> mData;
  };


  class VkDescriptorSet
  {

  private:
    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    std::vector<vk::DescriptorPoolSize> descriptorTypes;
  };
}

#endif
