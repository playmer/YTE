#pragma once

#ifndef YTE_Graphics_Vulkan_VkShaderDescriptions_hpp
#define YTE_Graphics_Vulkan_VkShaderDescriptions_hpp

#include <variant>

#include "fmt/format.h"

#include "YTE/Graphics/Generics/Shader.hpp"

#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"

#include "YTE/StandardLibrary/Utilities.hpp"
#include "YTE/Graphics/Vertex.hpp"

namespace YTE
{
  vk::Format to_vulkan(VertexFormat aFormat);
  vk::VertexInputRate to_vulkan(VertexInputRate aType);
  vk::DescriptorType to_vulkan(DescriptorType aType);
  vk::ImageLayout to_vulkan(ImageLayout aLayout);
  vk::ShaderStageFlags to_vulkan(ShaderStageFlags aFlags);

  class VkShaderDescriptions
  {
  public:
    using BufferOrImage = std::variant<std::shared_ptr<vkhlf::Buffer>, vkhlf::DescriptorImageInfo>;

    VkShaderDescriptions(size_t aNumberOfBindings = 2, size_t aNumberOfAttributes = 8)
    {
      mBindings.reserve(aNumberOfBindings);
      mAttributes.reserve(aNumberOfAttributes);
    }

    VkShaderDescriptions(ShaderDescriptions const& aDescriptions);

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

    void AddTextureDescriptor(vk::DescriptorType aDescriptorType, vk::ImageLayout aLayout, vk::ShaderStageFlagBits aStage)
    {
      vkhlf::DescriptorImageInfo descriptorTextureInfo{ nullptr, nullptr, aLayout };
      mWriteDescriptorSet.emplace_back(nullptr, mBufferBinding, 0, 1, aDescriptorType, descriptorTextureInfo, nullptr);
      mDescriptorSetLayout.emplace_back(mBufferBinding, aDescriptorType, aStage, nullptr);

      ++mBufferBinding;
    }

    void AddDescriptor(vk::DescriptorType aDescriptorType, vk::ShaderStageFlagBits aStage, size_t aBufferSize, size_t aBufferOffset = 0)
    {
      vkhlf::DescriptorBufferInfo descriptorBufferInfo{ nullptr, aBufferOffset, aBufferSize };
      mWriteDescriptorSet.emplace_back(nullptr, mBufferBinding, 0, 1, aDescriptorType, nullptr, descriptorBufferInfo);
      mDescriptorSetLayout.emplace_back(mBufferBinding, aDescriptorType, aStage, nullptr);

      ++mBufferBinding;
    }

    u32 CountDescriptorsOfType(vk::DescriptorType aType) const
    {
      u32 ofType{ 0 };

      for (auto const& descriptor : mDescriptorSetLayout)
      {
        if (descriptor.descriptorType == aType)
        {
          ++ofType;
        }
      }

      return ofType;
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

    std::vector<vkhlf::DescriptorSetLayoutBinding>& DescriptorSetLayout()
    {
      return mDescriptorSetLayout;
    }

    std::vector<vkhlf::WriteDescriptorSet> MakeWriteDescriptorSet(
      std::shared_ptr<vkhlf::DescriptorSet>* aDescriptorSet, 
      std::vector<BufferOrImage> aBuffersOrImages)
    {
      //assert(aBuffersOrImages.size() == mWriteDescriptorSet.size());
      auto newSet = mWriteDescriptorSet;

      for (auto [set, i] : enumerate(newSet))
      {
        if (auto buffer = std::get_if<std::shared_ptr<vkhlf::Buffer>>(&aBuffersOrImages[i]))
        {
          newSet[i].bufferInfo->buffer = *buffer;
        }
        else if (auto image = std::get_if<vkhlf::DescriptorImageInfo>(&aBuffersOrImages[i]))
        {
          newSet[i].imageInfo->sampler = image->sampler;
          newSet[i].imageInfo->imageView = image->imageView;
          newSet[i].imageInfo->imageLayout = image->imageLayout;
        }
        newSet[i].dstSet = *aDescriptorSet;
      }

      return newSet;
    }

  private:
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
    std::vector<vkhlf::WriteDescriptorSet> mWriteDescriptorSet;
    u32 mBufferBinding = 0;

  };
}

#endif
