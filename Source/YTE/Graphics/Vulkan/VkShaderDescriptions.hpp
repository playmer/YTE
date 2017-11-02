///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkShaderDescriptions_hpp
#define YTE_Graphics_Vulkan_VkShaderDescriptions_hpp

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
      toAdd.format = aFormat; // TODO: Do we need the alpha?
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



  private:
    std::vector<vk::VertexInputBindingDescription> mBindings;
    std::vector<vk::VertexInputAttributeDescription> mAttributes;
    u32 mBinding = 0;
    u32 mVertexOffset = 0;
    u32 mLocation = 0;
  };
}

#endif
