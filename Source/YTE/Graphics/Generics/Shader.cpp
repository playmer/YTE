#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Shader.hpp"

namespace YTE
{
  YTEDefineType(Shader)
  {
    RegisterType<Shader>();
    TypeBuilder<Shader> builder;
  }



  Shader::Shader(std::string &aName)
    : mShaderSetName(aName)
  {
  }


  ShaderDescriptions::ShaderDescriptions(size_t aNumberOfBindings, size_t aNumberOfAttributes)
  {
    mBindings.reserve(aNumberOfBindings);
    mAttributes.reserve(aNumberOfAttributes);
  }

  //void ShaderDescriptions::Append(ShaderDescriptions aDescriptions)
  //{
  //  auto const& bindings = aDescriptions.Bindings();
  //  auto const& attributes = aDescriptions.Attributes();
  //  auto const& descriptoSetLayouts = aDescriptions.DescriptorSetLayouts();
  //
  //  for (auto const& binding : bindings)
  //  {
  //    mBindings.emplace_back(mBinding + binding.binding,
  //      binding.stride,
  //      binding.inputRate);
  //  }
  //
  //  for (auto const& attribute : attributes)
  //  {
  //    mAttributes.emplace_back(attribute.location,
  //      attribute.binding,
  //      attribute.format,
  //      attribute.offset);
  //  }
  //
  //  for (auto const& dsl : descriptoSetLayouts)
  //  {
  //    auto const binding = dsl.mBinding;
  //    auto const descriptorType = dsl.mDescriptorType;
  //    auto const stage = dsl.mStageFlags;
  //
  //
  //    //switch (dsl.mType)
  //    //{
  //    //  case DescriptorSetLayoutBinding::Type::Image:
  //    //  {
  //    //    vkhlf::DescriptorImageInfo descriptorTextureInfo{ nullptr, nullptr, to_vulkan(dsl.mBufferOrImage.mLayout) };
  //    //    mWriteDescriptorSet.emplace_back(nullptr, binding, 0, 1, descriptorType, descriptorTextureInfo, nullptr);
  //    //    mDescriptorSetLayout.emplace_back(binding, descriptorType, stage, nullptr);
  //    //    break;
  //    //  }
  //    //  case DescriptorSetLayoutBinding::Type::Buffer:
  //    //  {
  //    //    auto const& buffer = dsl.mBufferOrImage.mBuffer;
  //    //  
  //    //    vkhlf::DescriptorBufferInfo descriptorBufferInfo{ nullptr, buffer.mOffset, buffer.mSize };
  //    //    mWriteDescriptorSet.emplace_back(nullptr, binding, 0, 1, descriptorType, nullptr, descriptorBufferInfo);
  //    //    mDescriptorSetLayout.emplace_back(binding, descriptorType, stage, nullptr);
  //    //    break;
  //    //  }
  //    //}
  //  }
  //  mLines = aDescriptions.Lines();
  //}

  u32 ShaderDescriptions::CountDescriptorsOfType(DescriptorType aType) const
  {
    u32 ofType{ 0 };

    for (auto const& descriptor : mDescriptorSetLayouts)
    {
      if (descriptor.mDescriptorType == aType)
      {
        ++ofType;
      }
    }

    return ofType;
  }
}
