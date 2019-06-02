#include "YTE/Graphics/Vulkan/VkShaderDescriptions.hpp"


namespace YTE
{
  VkShaderDescriptions::VkShaderDescriptions(ShaderDescriptions const& aDescriptions)
  {
    auto const& bindings = aDescriptions.Bindings();
    auto const& attributes = aDescriptions.Attributes();
    auto const& descriptoSetLayouts = aDescriptions.DescriptorSetLayouts();

    mBindings.reserve(bindings.size());
    mAttributes.reserve(attributes.size());
    mDescriptorSetLayout.reserve(descriptoSetLayouts.size());
    mWriteDescriptorSet.reserve(descriptoSetLayouts.size());


    for (auto const& binding : bindings)
    {
      mBindings.emplace_back(binding.binding, binding.stride, to_vulkan(binding.inputRate));
    }

    for (auto const& attribute : attributes)
    {
      mAttributes.emplace_back(attribute.location, attribute.binding, to_vulkan(attribute.format), attribute.offset);
    }

    for (auto const& dsl : descriptoSetLayouts)
    {
      auto const binding = dsl.mBinding;
      auto const descriptorType = to_vulkan(dsl.mDescriptorType);
      auto const stage = to_vulkan(dsl.mStageFlags);


      switch (dsl.mType)
      {
        case DescriptorSetLayoutBinding::Type::Image:
        {
          vkhlf::DescriptorImageInfo descriptorTextureInfo{ nullptr, nullptr, to_vulkan(dsl.mBufferOrImage.mLayout) };
          mWriteDescriptorSet.emplace_back(nullptr, binding, 0, 1, descriptorType, descriptorTextureInfo, nullptr);
          mDescriptorSetLayout.emplace_back(binding, descriptorType, stage, nullptr);
          break;
        }
        case DescriptorSetLayoutBinding::Type::Buffer:
        {
          auto const& buffer = dsl.mBufferOrImage.mBuffer;

          vkhlf::DescriptorBufferInfo descriptorBufferInfo{nullptr, buffer.mOffset, buffer.mSize };
          mWriteDescriptorSet.emplace_back(nullptr, binding, 0, 1, descriptorType, nullptr, descriptorBufferInfo);
          mDescriptorSetLayout.emplace_back(binding, descriptorType, stage, nullptr);
          break;
        }
      }
    }

    mLines = aDescriptions.Lines();
  }

  vk::Format to_vulkan(VertexFormat aFormat)
  {
    switch (aFormat)
    {
    case VertexFormat::Undefined: return vk::Format::eUndefined;
    case VertexFormat::R4G4UnormPack8: return vk::Format::eR4G4UnormPack8;
    case VertexFormat::R4G4B4A4UnormPack16: return vk::Format::eR4G4B4A4UnormPack16;
    case VertexFormat::B4G4R4A4UnormPack16: return vk::Format::eB4G4R4A4UnormPack16;
    case VertexFormat::R5G6B5UnormPack16: return vk::Format::eR5G6B5UnormPack16;
    case VertexFormat::B5G6R5UnormPack16: return vk::Format::eB5G6R5UnormPack16;
    case VertexFormat::R5G5B5A1UnormPack16: return vk::Format::eR5G5B5A1UnormPack16;
    case VertexFormat::B5G5R5A1UnormPack16: return vk::Format::eB5G5R5A1UnormPack16;
    case VertexFormat::A1R5G5B5UnormPack16: return vk::Format::eA1R5G5B5UnormPack16;
    case VertexFormat::R8Unorm: return vk::Format::eR8Unorm;
    case VertexFormat::R8Snorm: return vk::Format::eR8Snorm;
    case VertexFormat::R8Uscaled: return vk::Format::eR8Uscaled;
    case VertexFormat::R8Sscaled: return vk::Format::eR8Sscaled;
    case VertexFormat::R8Uint: return vk::Format::eR8Uint;
    case VertexFormat::R8Sint: return vk::Format::eR8Sint;
    case VertexFormat::R8Srgb: return vk::Format::eR8Srgb;
    case VertexFormat::R8G8Unorm: return vk::Format::eR8G8Unorm;
    case VertexFormat::R8G8Snorm: return vk::Format::eR8G8Snorm;
    case VertexFormat::R8G8Uscaled: return vk::Format::eR8G8Uscaled;
    case VertexFormat::R8G8Sscaled: return vk::Format::eR8G8Sscaled;
    case VertexFormat::R8G8Uint: return vk::Format::eR8G8Uint;
    case VertexFormat::R8G8Sint: return vk::Format::eR8G8Sint;
    case VertexFormat::R8G8Srgb: return vk::Format::eR8G8Srgb;
    case VertexFormat::R8G8B8Unorm: return vk::Format::eR8G8B8Unorm;
    case VertexFormat::R8G8B8Snorm: return vk::Format::eR8G8B8Snorm;
    case VertexFormat::R8G8B8Uscaled: return vk::Format::eR8G8B8Uscaled;
    case VertexFormat::R8G8B8Sscaled: return vk::Format::eR8G8B8Sscaled;
    case VertexFormat::R8G8B8Uint: return vk::Format::eR8G8B8Uint;
    case VertexFormat::R8G8B8Sint: return vk::Format::eR8G8B8Sint;
    case VertexFormat::R8G8B8Srgb: return vk::Format::eR8G8B8Srgb;
    case VertexFormat::B8G8R8Unorm: return vk::Format::eB8G8R8Unorm;
    case VertexFormat::B8G8R8Snorm: return vk::Format::eB8G8R8Snorm;
    case VertexFormat::B8G8R8Uscaled: return vk::Format::eB8G8R8Uscaled;
    case VertexFormat::B8G8R8Sscaled: return vk::Format::eB8G8R8Sscaled;
    case VertexFormat::B8G8R8Uint: return vk::Format::eB8G8R8Uint;
    case VertexFormat::B8G8R8Sint: return vk::Format::eB8G8R8Sint;
    case VertexFormat::B8G8R8Srgb: return vk::Format::eB8G8R8Srgb;
    case VertexFormat::R8G8B8A8Unorm: return vk::Format::eR8G8B8A8Unorm;
    case VertexFormat::R8G8B8A8Snorm: return vk::Format::eR8G8B8A8Snorm;
    case VertexFormat::R8G8B8A8Uscaled: return vk::Format::eR8G8B8A8Uscaled;
    case VertexFormat::R8G8B8A8Sscaled: return vk::Format::eR8G8B8A8Sscaled;
    case VertexFormat::R8G8B8A8Uint: return vk::Format::eR8G8B8A8Uint;
    case VertexFormat::R8G8B8A8Sint: return vk::Format::eR8G8B8A8Sint;
    case VertexFormat::R8G8B8A8Srgb: return vk::Format::eR8G8B8A8Srgb;
    case VertexFormat::B8G8R8A8Unorm: return vk::Format::eB8G8R8A8Unorm;
    case VertexFormat::B8G8R8A8Snorm: return vk::Format::eB8G8R8A8Snorm;
    case VertexFormat::B8G8R8A8Uscaled: return vk::Format::eB8G8R8A8Uscaled;
    case VertexFormat::B8G8R8A8Sscaled: return vk::Format::eB8G8R8A8Sscaled;
    case VertexFormat::B8G8R8A8Uint: return vk::Format::eB8G8R8A8Uint;
    case VertexFormat::B8G8R8A8Sint: return vk::Format::eB8G8R8A8Sint;
    case VertexFormat::B8G8R8A8Srgb: return vk::Format::eB8G8R8A8Srgb;
    case VertexFormat::A8B8G8R8UnormPack32: return vk::Format::eA8B8G8R8UnormPack32;
    case VertexFormat::A8B8G8R8SnormPack32: return vk::Format::eA8B8G8R8SnormPack32;
    case VertexFormat::A8B8G8R8UscaledPack32: return vk::Format::eA8B8G8R8UscaledPack32;
    case VertexFormat::A8B8G8R8SscaledPack32: return vk::Format::eA8B8G8R8SscaledPack32;
    case VertexFormat::A8B8G8R8UintPack32: return vk::Format::eA8B8G8R8UintPack32;
    case VertexFormat::A8B8G8R8SintPack32: return vk::Format::eA8B8G8R8SintPack32;
    case VertexFormat::A8B8G8R8SrgbPack32: return vk::Format::eA8B8G8R8SrgbPack32;
    case VertexFormat::A2R10G10B10UnormPack32: return vk::Format::eA2R10G10B10UnormPack32;
    case VertexFormat::A2R10G10B10SnormPack32: return vk::Format::eA2R10G10B10SnormPack32;
    case VertexFormat::A2R10G10B10UscaledPack32: return vk::Format::eA2R10G10B10UscaledPack32;
    case VertexFormat::A2R10G10B10SscaledPack32: return vk::Format::eA2R10G10B10SscaledPack32;
    case VertexFormat::A2R10G10B10UintPack32: return vk::Format::eA2R10G10B10UintPack32;
    case VertexFormat::A2R10G10B10SintPack32: return vk::Format::eA2R10G10B10SintPack32;
    case VertexFormat::A2B10G10R10UnormPack32: return vk::Format::eA2B10G10R10UnormPack32;
    case VertexFormat::A2B10G10R10SnormPack32: return vk::Format::eA2B10G10R10SnormPack32;
    case VertexFormat::A2B10G10R10UscaledPack32: return vk::Format::eA2B10G10R10UscaledPack32;
    case VertexFormat::A2B10G10R10SscaledPack32: return vk::Format::eA2B10G10R10SscaledPack32;
    case VertexFormat::A2B10G10R10UintPack32: return vk::Format::eA2B10G10R10UintPack32;
    case VertexFormat::A2B10G10R10SintPack32: return vk::Format::eA2B10G10R10SintPack32;
    case VertexFormat::R16Unorm: return vk::Format::eR16Unorm;
    case VertexFormat::R16Snorm: return vk::Format::eR16Snorm;
    case VertexFormat::R16Uscaled: return vk::Format::eR16Uscaled;
    case VertexFormat::R16Sscaled: return vk::Format::eR16Sscaled;
    case VertexFormat::R16Uint: return vk::Format::eR16Uint;
    case VertexFormat::R16Sint: return vk::Format::eR16Sint;
    case VertexFormat::R16Sfloat: return vk::Format::eR16Sfloat;
    case VertexFormat::R16G16Unorm: return vk::Format::eR16G16Unorm;
    case VertexFormat::R16G16Snorm: return vk::Format::eR16G16Snorm;
    case VertexFormat::R16G16Uscaled: return vk::Format::eR16G16Uscaled;
    case VertexFormat::R16G16Sscaled: return vk::Format::eR16G16Sscaled;
    case VertexFormat::R16G16Uint: return vk::Format::eR16G16Uint;
    case VertexFormat::R16G16Sint: return vk::Format::eR16G16Sint;
    case VertexFormat::R16G16Sfloat: return vk::Format::eR16G16Sfloat;
    case VertexFormat::R16G16B16Unorm: return vk::Format::eR16G16B16Unorm;
    case VertexFormat::R16G16B16Snorm: return vk::Format::eR16G16B16Snorm;
    case VertexFormat::R16G16B16Uscaled: return vk::Format::eR16G16B16Uscaled;
    case VertexFormat::R16G16B16Sscaled: return vk::Format::eR16G16B16Sscaled;
    case VertexFormat::R16G16B16Uint: return vk::Format::eR16G16B16Uint;
    case VertexFormat::R16G16B16Sint: return vk::Format::eR16G16B16Sint;
    case VertexFormat::R16G16B16Sfloat: return vk::Format::eR16G16B16Sfloat;
    case VertexFormat::R16G16B16A16Unorm: return vk::Format::eR16G16B16A16Unorm;
    case VertexFormat::R16G16B16A16Snorm: return vk::Format::eR16G16B16A16Snorm;
    case VertexFormat::R16G16B16A16Uscaled: return vk::Format::eR16G16B16A16Uscaled;
    case VertexFormat::R16G16B16A16Sscaled: return vk::Format::eR16G16B16A16Sscaled;
    case VertexFormat::R16G16B16A16Uint: return vk::Format::eR16G16B16A16Uint;
    case VertexFormat::R16G16B16A16Sint: return vk::Format::eR16G16B16A16Sint;
    case VertexFormat::R16G16B16A16Sfloat: return vk::Format::eR16G16B16A16Sfloat;
    case VertexFormat::R32Uint: return vk::Format::eR32Uint;
    case VertexFormat::R32Sint: return vk::Format::eR32Sint;
    case VertexFormat::R32Sfloat: return vk::Format::eR32Sfloat;
    case VertexFormat::R32G32Uint: return vk::Format::eR32G32Uint;
    case VertexFormat::R32G32Sint: return vk::Format::eR32G32Sint;
    case VertexFormat::R32G32Sfloat: return vk::Format::eR32G32Sfloat;
    case VertexFormat::R32G32B32Uint: return vk::Format::eR32G32B32Uint;
    case VertexFormat::R32G32B32Sint: return vk::Format::eR32G32B32Sint;
    case VertexFormat::R32G32B32Sfloat: return vk::Format::eR32G32B32Sfloat;
    case VertexFormat::R32G32B32A32Uint: return vk::Format::eR32G32B32A32Uint;
    case VertexFormat::R32G32B32A32Sint: return vk::Format::eR32G32B32A32Sint;
    case VertexFormat::R32G32B32A32Sfloat: return vk::Format::eR32G32B32A32Sfloat;
    case VertexFormat::R64Uint: return vk::Format::eR64Uint;
    case VertexFormat::R64Sint: return vk::Format::eR64Sint;
    case VertexFormat::R64Sfloat: return vk::Format::eR64Sfloat;
    case VertexFormat::R64G64Uint: return vk::Format::eR64G64Uint;
    case VertexFormat::R64G64Sint: return vk::Format::eR64G64Sint;
    case VertexFormat::R64G64Sfloat: return vk::Format::eR64G64Sfloat;
    case VertexFormat::R64G64B64Uint: return vk::Format::eR64G64B64Uint;
    case VertexFormat::R64G64B64Sint: return vk::Format::eR64G64B64Sint;
    case VertexFormat::R64G64B64Sfloat: return vk::Format::eR64G64B64Sfloat;
    case VertexFormat::R64G64B64A64Uint: return vk::Format::eR64G64B64A64Uint;
    case VertexFormat::R64G64B64A64Sint: return vk::Format::eR64G64B64A64Sint;
    case VertexFormat::R64G64B64A64Sfloat: return vk::Format::eR64G64B64A64Sfloat;
    case VertexFormat::B10G11R11UfloatPack32: return vk::Format::eB10G11R11UfloatPack32;
    case VertexFormat::E5B9G9R9UfloatPack32: return vk::Format::eE5B9G9R9UfloatPack32;
    case VertexFormat::D16Unorm: return vk::Format::eD16Unorm;
    case VertexFormat::X8D24UnormPack32: return vk::Format::eX8D24UnormPack32;
    case VertexFormat::D32Sfloat: return vk::Format::eD32Sfloat;
    case VertexFormat::S8Uint: return vk::Format::eS8Uint;
    case VertexFormat::D16UnormS8Uint: return vk::Format::eD16UnormS8Uint;
    case VertexFormat::D24UnormS8Uint: return vk::Format::eD24UnormS8Uint;
    case VertexFormat::D32SfloatS8Uint: return vk::Format::eD32SfloatS8Uint;
    case VertexFormat::Bc1RgbUnormBlock: return vk::Format::eBc1RgbUnormBlock;
    case VertexFormat::Bc1RgbSrgbBlock: return vk::Format::eBc1RgbSrgbBlock;
    case VertexFormat::Bc1RgbaUnormBlock: return vk::Format::eBc1RgbaUnormBlock;
    case VertexFormat::Bc1RgbaSrgbBlock: return vk::Format::eBc1RgbaSrgbBlock;
    case VertexFormat::Bc2UnormBlock: return vk::Format::eBc2UnormBlock;
    case VertexFormat::Bc2SrgbBlock: return vk::Format::eBc2SrgbBlock;
    case VertexFormat::Bc3UnormBlock: return vk::Format::eBc3UnormBlock;
    case VertexFormat::Bc3SrgbBlock: return vk::Format::eBc3SrgbBlock;
    case VertexFormat::Bc4UnormBlock: return vk::Format::eBc4UnormBlock;
    case VertexFormat::Bc4SnormBlock: return vk::Format::eBc4SnormBlock;
    case VertexFormat::Bc5UnormBlock: return vk::Format::eBc5UnormBlock;
    case VertexFormat::Bc5SnormBlock: return vk::Format::eBc5SnormBlock;
    case VertexFormat::Bc6HUfloatBlock: return vk::Format::eBc6HUfloatBlock;
    case VertexFormat::Bc6HSfloatBlock: return vk::Format::eBc6HSfloatBlock;
    case VertexFormat::Bc7UnormBlock: return vk::Format::eBc7UnormBlock;
    case VertexFormat::Bc7SrgbBlock: return vk::Format::eBc7SrgbBlock;
    case VertexFormat::Etc2R8G8B8UnormBlock: return vk::Format::eEtc2R8G8B8UnormBlock;
    case VertexFormat::Etc2R8G8B8SrgbBlock: return vk::Format::eEtc2R8G8B8SrgbBlock;
    case VertexFormat::Etc2R8G8B8A1UnormBlock: return vk::Format::eEtc2R8G8B8A1UnormBlock;
    case VertexFormat::Etc2R8G8B8A1SrgbBlock: return vk::Format::eEtc2R8G8B8A1SrgbBlock;
    case VertexFormat::Etc2R8G8B8A8UnormBlock: return vk::Format::eEtc2R8G8B8A8UnormBlock;
    case VertexFormat::Etc2R8G8B8A8SrgbBlock: return vk::Format::eEtc2R8G8B8A8SrgbBlock;
    case VertexFormat::EacR11UnormBlock: return vk::Format::eEacR11UnormBlock;
    case VertexFormat::EacR11SnormBlock: return vk::Format::eEacR11SnormBlock;
    case VertexFormat::EacR11G11UnormBlock: return vk::Format::eEacR11G11UnormBlock;
    case VertexFormat::EacR11G11SnormBlock: return vk::Format::eEacR11G11SnormBlock;
    case VertexFormat::Astc4x4UnormBlock: return vk::Format::eAstc4x4UnormBlock;
    case VertexFormat::Astc4x4SrgbBlock: return vk::Format::eAstc4x4SrgbBlock;
    case VertexFormat::Astc5x4UnormBlock: return vk::Format::eAstc5x4UnormBlock;
    case VertexFormat::Astc5x4SrgbBlock: return vk::Format::eAstc5x4SrgbBlock;
    case VertexFormat::Astc5x5UnormBlock: return vk::Format::eAstc5x5UnormBlock;
    case VertexFormat::Astc5x5SrgbBlock: return vk::Format::eAstc5x5SrgbBlock;
    case VertexFormat::Astc6x5UnormBlock: return vk::Format::eAstc6x5UnormBlock;
    case VertexFormat::Astc6x5SrgbBlock: return vk::Format::eAstc6x5SrgbBlock;
    case VertexFormat::Astc6x6UnormBlock: return vk::Format::eAstc6x6UnormBlock;
    case VertexFormat::Astc6x6SrgbBlock: return vk::Format::eAstc6x6SrgbBlock;
    case VertexFormat::Astc8x5UnormBlock: return vk::Format::eAstc8x5UnormBlock;
    case VertexFormat::Astc8x5SrgbBlock: return vk::Format::eAstc8x5SrgbBlock;
    case VertexFormat::Astc8x6UnormBlock: return vk::Format::eAstc8x6UnormBlock;
    case VertexFormat::Astc8x6SrgbBlock: return vk::Format::eAstc8x6SrgbBlock;
    case VertexFormat::Astc8x8UnormBlock: return vk::Format::eAstc8x8UnormBlock;
    case VertexFormat::Astc8x8SrgbBlock: return vk::Format::eAstc8x8SrgbBlock;
    case VertexFormat::Astc10x5UnormBlock: return vk::Format::eAstc10x5UnormBlock;
    case VertexFormat::Astc10x5SrgbBlock: return vk::Format::eAstc10x5SrgbBlock;
    case VertexFormat::Astc10x6UnormBlock: return vk::Format::eAstc10x6UnormBlock;
    case VertexFormat::Astc10x6SrgbBlock: return vk::Format::eAstc10x6SrgbBlock;
    case VertexFormat::Astc10x8UnormBlock: return vk::Format::eAstc10x8UnormBlock;
    case VertexFormat::Astc10x8SrgbBlock: return vk::Format::eAstc10x8SrgbBlock;
    case VertexFormat::Astc10x10UnormBlock: return vk::Format::eAstc10x10UnormBlock;
    case VertexFormat::Astc10x10SrgbBlock: return vk::Format::eAstc10x10SrgbBlock;
    case VertexFormat::Astc12x10UnormBlock: return vk::Format::eAstc12x10UnormBlock;
    case VertexFormat::Astc12x10SrgbBlock: return vk::Format::eAstc12x10SrgbBlock;
    case VertexFormat::Astc12x12UnormBlock: return vk::Format::eAstc12x12UnormBlock;
    case VertexFormat::Astc12x12SrgbBlock: return vk::Format::eAstc12x12SrgbBlock;
    case VertexFormat::G8B8G8R8422Unorm: return vk::Format::eG8B8G8R8422Unorm;
    case VertexFormat::B8G8R8G8422Unorm: return vk::Format::eB8G8R8G8422Unorm;
    case VertexFormat::G8B8R83Plane420Unorm: return vk::Format::eG8B8R83Plane420Unorm;
    case VertexFormat::G8B8R82Plane420Unorm: return vk::Format::eG8B8R82Plane420Unorm;
    case VertexFormat::G8B8R83Plane422Unorm: return vk::Format::eG8B8R83Plane422Unorm;
    case VertexFormat::G8B8R82Plane422Unorm: return vk::Format::eG8B8R82Plane422Unorm;
    case VertexFormat::G8B8R83Plane444Unorm: return vk::Format::eG8B8R83Plane444Unorm;
    case VertexFormat::R10X6UnormPack16: return vk::Format::eR10X6UnormPack16;
    case VertexFormat::R10X6G10X6Unorm2Pack16: return vk::Format::eR10X6G10X6Unorm2Pack16;
    case VertexFormat::R10X6G10X6B10X6A10X6Unorm4Pack16: return vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16;
    case VertexFormat::G10X6B10X6G10X6R10X6422Unorm4Pack16: return vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16;
    case VertexFormat::B10X6G10X6R10X6G10X6422Unorm4Pack16: return vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16;
    case VertexFormat::G10X6B10X6R10X63Plane420Unorm3Pack16: return vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16;
    case VertexFormat::G10X6B10X6R10X62Plane420Unorm3Pack16: return vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16;
    case VertexFormat::G10X6B10X6R10X63Plane422Unorm3Pack16: return vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16;
    case VertexFormat::G10X6B10X6R10X62Plane422Unorm3Pack16: return vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16;
    case VertexFormat::G10X6B10X6R10X63Plane444Unorm3Pack16: return vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16;
    case VertexFormat::R12X4UnormPack16: return vk::Format::eR12X4UnormPack16;
    case VertexFormat::R12X4G12X4Unorm2Pack16: return vk::Format::eR12X4G12X4Unorm2Pack16;
    case VertexFormat::R12X4G12X4B12X4A12X4Unorm4Pack16: return vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16;
    case VertexFormat::G12X4B12X4G12X4R12X4422Unorm4Pack16: return vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16;
    case VertexFormat::B12X4G12X4R12X4G12X4422Unorm4Pack16: return vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16;
    case VertexFormat::G12X4B12X4R12X43Plane420Unorm3Pack16: return vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16;
    case VertexFormat::G12X4B12X4R12X42Plane420Unorm3Pack16: return vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16;
    case VertexFormat::G12X4B12X4R12X43Plane422Unorm3Pack16: return vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16;
    case VertexFormat::G12X4B12X4R12X42Plane422Unorm3Pack16: return vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16;
    case VertexFormat::G12X4B12X4R12X43Plane444Unorm3Pack16: return vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16;
    case VertexFormat::G16B16G16R16422Unorm: return vk::Format::eG16B16G16R16422Unorm;
    case VertexFormat::B16G16R16G16422Unorm: return vk::Format::eB16G16R16G16422Unorm;
    case VertexFormat::G16B16R163Plane420Unorm: return vk::Format::eG16B16R163Plane420Unorm;
    case VertexFormat::G16B16R162Plane420Unorm: return vk::Format::eG16B16R162Plane420Unorm;
    case VertexFormat::G16B16R163Plane422Unorm: return vk::Format::eG16B16R163Plane422Unorm;
    case VertexFormat::G16B16R162Plane422Unorm: return vk::Format::eG16B16R162Plane422Unorm;
    case VertexFormat::G16B16R163Plane444Unorm: return vk::Format::eG16B16R163Plane444Unorm;
    case VertexFormat::Pvrtc12BppUnormBlockIMG: return vk::Format::ePvrtc12BppUnormBlockIMG;
    case VertexFormat::Pvrtc14BppUnormBlockIMG: return vk::Format::ePvrtc14BppUnormBlockIMG;
    case VertexFormat::Pvrtc22BppUnormBlockIMG: return vk::Format::ePvrtc22BppUnormBlockIMG;
    case VertexFormat::Pvrtc24BppUnormBlockIMG: return vk::Format::ePvrtc24BppUnormBlockIMG;
    case VertexFormat::Pvrtc12BppSrgbBlockIMG: return vk::Format::ePvrtc12BppSrgbBlockIMG;
    case VertexFormat::Pvrtc14BppSrgbBlockIMG: return vk::Format::ePvrtc14BppSrgbBlockIMG;
    case VertexFormat::Pvrtc22BppSrgbBlockIMG: return vk::Format::ePvrtc22BppSrgbBlockIMG;
    case VertexFormat::Pvrtc24BppSrgbBlockIMG: return vk::Format::ePvrtc24BppSrgbBlockIMG;
    case VertexFormat::G8B8G8R8422UnormKHR: return vk::Format::eG8B8G8R8422UnormKHR;
    case VertexFormat::B8G8R8G8422UnormKHR: return vk::Format::eB8G8R8G8422UnormKHR;
    case VertexFormat::G8B8R83Plane420UnormKHR: return vk::Format::eG8B8R83Plane420UnormKHR;
    case VertexFormat::G8B8R82Plane420UnormKHR: return vk::Format::eG8B8R82Plane420UnormKHR;
    case VertexFormat::G8B8R83Plane422UnormKHR: return vk::Format::eG8B8R83Plane422UnormKHR;
    case VertexFormat::G8B8R82Plane422UnormKHR: return vk::Format::eG8B8R82Plane422UnormKHR;
    case VertexFormat::G8B8R83Plane444UnormKHR: return vk::Format::eG8B8R83Plane444UnormKHR;
    case VertexFormat::R10X6UnormPack16KHR: return vk::Format::eR10X6UnormPack16KHR;
    case VertexFormat::R10X6G10X6Unorm2Pack16KHR: return vk::Format::eR10X6G10X6Unorm2Pack16KHR;
    case VertexFormat::R10X6G10X6B10X6A10X6Unorm4Pack16KHR: return vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16KHR;
    case VertexFormat::G10X6B10X6G10X6R10X6422Unorm4Pack16KHR: return vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR;
    case VertexFormat::B10X6G10X6R10X6G10X6422Unorm4Pack16KHR: return vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR;
    case VertexFormat::G10X6B10X6R10X63Plane420Unorm3Pack16KHR: return vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16KHR;
    case VertexFormat::G10X6B10X6R10X62Plane420Unorm3Pack16KHR: return vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16KHR;
    case VertexFormat::G10X6B10X6R10X63Plane422Unorm3Pack16KHR: return vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16KHR;
    case VertexFormat::G10X6B10X6R10X62Plane422Unorm3Pack16KHR: return vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16KHR;
    case VertexFormat::G10X6B10X6R10X63Plane444Unorm3Pack16KHR: return vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16KHR;
    case VertexFormat::R12X4UnormPack16KHR: return vk::Format::eR12X4UnormPack16KHR;
    case VertexFormat::R12X4G12X4Unorm2Pack16KHR: return vk::Format::eR12X4G12X4Unorm2Pack16KHR;
    case VertexFormat::R12X4G12X4B12X4A12X4Unorm4Pack16KHR: return vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16KHR;
    case VertexFormat::G12X4B12X4G12X4R12X4422Unorm4Pack16KHR: return vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR;
    case VertexFormat::B12X4G12X4R12X4G12X4422Unorm4Pack16KHR: return vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR;
    case VertexFormat::G12X4B12X4R12X43Plane420Unorm3Pack16KHR: return vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16KHR;
    case VertexFormat::G12X4B12X4R12X42Plane420Unorm3Pack16KHR: return vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16KHR;
    case VertexFormat::G12X4B12X4R12X43Plane422Unorm3Pack16KHR: return vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16KHR;
    case VertexFormat::G12X4B12X4R12X42Plane422Unorm3Pack16KHR: return vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16KHR;
    case VertexFormat::G12X4B12X4R12X43Plane444Unorm3Pack16KHR: return vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16KHR;
    case VertexFormat::G16B16G16R16422UnormKHR: return vk::Format::eG16B16G16R16422UnormKHR;
    case VertexFormat::B16G16R16G16422UnormKHR: return vk::Format::eB16G16R16G16422UnormKHR;
    case VertexFormat::G16B16R163Plane420UnormKHR: return vk::Format::eG16B16R163Plane420UnormKHR;
    case VertexFormat::G16B16R162Plane420UnormKHR: return vk::Format::eG16B16R162Plane420UnormKHR;
    case VertexFormat::G16B16R163Plane422UnormKHR: return vk::Format::eG16B16R163Plane422UnormKHR;
    case VertexFormat::G16B16R162Plane422UnormKHR: return vk::Format::eG16B16R162Plane422UnormKHR;
    case VertexFormat::G16B16R163Plane444UnormKHR: return vk::Format::eG16B16R163Plane444UnormKHR;
    }

    __debugbreak();

    return vk::Format::eUndefined;
  }

  vk::VertexInputRate to_vulkan(VertexInputRate aType)
  {
    switch (aType)
    {
    case VertexInputRate::Vertex: return vk::VertexInputRate::eVertex;
    case VertexInputRate::Instance: return vk::VertexInputRate::eInstance;
    };

    __debugbreak();
    return {};
  }

  vk::DescriptorType to_vulkan(DescriptorType aType)
  {
    switch (aType)
    {
    case DescriptorType::Sampler: return vk::DescriptorType::eSampler;
    case DescriptorType::CombinedImageSampler: return vk::DescriptorType::eCombinedImageSampler;
    case DescriptorType::SampledImage: return vk::DescriptorType::eSampledImage;
    case DescriptorType::StorageImage: return vk::DescriptorType::eStorageImage;
    case DescriptorType::UniformTexelBuffer: return vk::DescriptorType::eUniformTexelBuffer;
    case DescriptorType::StorageTexelBuffer: return vk::DescriptorType::eStorageTexelBuffer;
    case DescriptorType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
    case DescriptorType::StorageBuffer: return vk::DescriptorType::eStorageBuffer;
    case DescriptorType::UniformBufferDynamic: return vk::DescriptorType::eUniformBufferDynamic;
    case DescriptorType::StorageBufferDynamic: return vk::DescriptorType::eStorageBufferDynamic;
    case DescriptorType::InputAttachment: return vk::DescriptorType::eInputAttachment;
    case DescriptorType::InlineUniformBlockEXT: return vk::DescriptorType::eInlineUniformBlockEXT;
    case DescriptorType::AccelerationStructureNV: return vk::DescriptorType::eAccelerationStructureNV;
    };

    __debugbreak();
    return {};
  }

  vk::ImageLayout to_vulkan(ImageLayout aLayout)
  {
    switch (aLayout)
    {
    case ImageLayout::Undefined: return vk::ImageLayout::eUndefined;
    case ImageLayout::General: return vk::ImageLayout::eGeneral;
    case ImageLayout::ColorAttachmentOptimal: return vk::ImageLayout::eColorAttachmentOptimal;
    case ImageLayout::DepthStencilAttachmentOptimal: return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case ImageLayout::DepthStencilReadOnlyOptimal: return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
    case ImageLayout::ShaderReadOnlyOptimal: return vk::ImageLayout::eShaderReadOnlyOptimal;
    case ImageLayout::TransferSrcOptimal: return vk::ImageLayout::eTransferSrcOptimal;
    case ImageLayout::TransferDstOptimal: return vk::ImageLayout::eTransferDstOptimal;
    case ImageLayout::Preinitialized: return vk::ImageLayout::ePreinitialized;
    case ImageLayout::DepthReadOnlyStencilAttachmentOptimal: return vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal;
    case ImageLayout::DepthAttachmentStencilReadOnlyOptimal: return vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
    case ImageLayout::PresentSrcKHR: return vk::ImageLayout::ePresentSrcKHR;
    case ImageLayout::SharedPresentKHR: return vk::ImageLayout::eSharedPresentKHR;
    case ImageLayout::ShadingRateOptimalNV: return vk::ImageLayout::eShadingRateOptimalNV;
    case ImageLayout::FragmentDensityMapOptimalEXT: return vk::ImageLayout::eFragmentDensityMapOptimalEXT;
    case ImageLayout::DepthReadOnlyStencilAttachmentOptimalKHR: return vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimalKHR;
    case ImageLayout::DepthAttachmentStencilReadOnlyOptimalKHR: return vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimalKHR;
    }

    __debugbreak();
    return vk::ImageLayout::eUndefined;
  }

  vk::ShaderStageFlags to_vulkan(ShaderStageFlags aFlags)
  {
    vk::ShaderStageFlags toReturn{0};

    if (aFlags & ShaderStageFlags::Vertex) toReturn |= vk::ShaderStageFlagBits::eVertex;
    if (aFlags & ShaderStageFlags::TessellationControl) toReturn |= vk::ShaderStageFlagBits::eTessellationControl;
    if (aFlags & ShaderStageFlags::TessellationEvaluation) toReturn |= vk::ShaderStageFlagBits::eTessellationEvaluation;
    if (aFlags & ShaderStageFlags::Geometry) toReturn |= vk::ShaderStageFlagBits::eGeometry;
    if (aFlags & ShaderStageFlags::Fragment) toReturn |= vk::ShaderStageFlagBits::eFragment;
    if (aFlags & ShaderStageFlags::Compute) toReturn |= vk::ShaderStageFlagBits::eCompute;
    //if (aFlags & ShaderStageFlags::AllGraphics) toReturn |= vk::ShaderStageFlagBits::eAllGraphics;
    //if (aFlags & ShaderStageFlags::All) toReturn |= vk::ShaderStageFlagBits::eAll;
    if (aFlags & ShaderStageFlags::RaygenNV) toReturn |= vk::ShaderStageFlagBits::eRaygenNV;
    if (aFlags & ShaderStageFlags::AnyHitNV) toReturn |= vk::ShaderStageFlagBits::eAnyHitNV;
    if (aFlags & ShaderStageFlags::ClosestHitNV) toReturn |= vk::ShaderStageFlagBits::eClosestHitNV;
    if (aFlags & ShaderStageFlags::MissNV) toReturn |= vk::ShaderStageFlagBits::eMissNV;
    if (aFlags & ShaderStageFlags::IntersectionNV) toReturn |= vk::ShaderStageFlagBits::eIntersectionNV;
    if (aFlags & ShaderStageFlags::CallableNV) toReturn |= vk::ShaderStageFlagBits::eCallableNV;
    if (aFlags & ShaderStageFlags::TaskNV) toReturn |= vk::ShaderStageFlagBits::eTaskNV;
    if (aFlags & ShaderStageFlags::MeshNV) toReturn |= vk::ShaderStageFlagBits::eMeshNV;

    return toReturn;
  }
}