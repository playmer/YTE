#pragma once

#ifndef YTE_Graphics_Generics_Shader_hpp
#define YTE_Graphics_Generics_Shader_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  enum class ShaderStage
  {
    Compute,
    Tessellation,
    Vertex,
    Fragment
  };

  class Shader : public EventHandler
  {
  public:
    YTEDeclareType(Shader);

    Shader(std::string &aName);

    virtual void Reload() {}

    virtual ~Shader() = default;



  protected:
    std::string mShaderSetName;
  };

  enum class VertexFormat
  {
    Undefined,
    R4G4UnormPack8,
    R4G4B4A4UnormPack16,
    B4G4R4A4UnormPack16,
    R5G6B5UnormPack16,
    B5G6R5UnormPack16,
    R5G5B5A1UnormPack16,
    B5G5R5A1UnormPack16,
    A1R5G5B5UnormPack16,
    R8Unorm,
    R8Snorm,
    R8Uscaled,
    R8Sscaled,
    R8Uint,
    R8Sint,
    R8Srgb,
    R8G8Unorm,
    R8G8Snorm,
    R8G8Uscaled,
    R8G8Sscaled,
    R8G8Uint,
    R8G8Sint,
    R8G8Srgb,
    R8G8B8Unorm,
    R8G8B8Snorm,
    R8G8B8Uscaled,
    R8G8B8Sscaled,
    R8G8B8Uint,
    R8G8B8Sint,
    R8G8B8Srgb,
    B8G8R8Unorm,
    B8G8R8Snorm,
    B8G8R8Uscaled,
    B8G8R8Sscaled,
    B8G8R8Uint,
    B8G8R8Sint,
    B8G8R8Srgb,
    R8G8B8A8Unorm,
    R8G8B8A8Snorm,
    R8G8B8A8Uscaled,
    R8G8B8A8Sscaled,
    R8G8B8A8Uint,
    R8G8B8A8Sint,
    R8G8B8A8Srgb,
    B8G8R8A8Unorm,
    B8G8R8A8Snorm,
    B8G8R8A8Uscaled,
    B8G8R8A8Sscaled,
    B8G8R8A8Uint,
    B8G8R8A8Sint,
    B8G8R8A8Srgb,
    A8B8G8R8UnormPack32,
    A8B8G8R8SnormPack32,
    A8B8G8R8UscaledPack32,
    A8B8G8R8SscaledPack32,
    A8B8G8R8UintPack32,
    A8B8G8R8SintPack32,
    A8B8G8R8SrgbPack32,
    A2R10G10B10UnormPack32,
    A2R10G10B10SnormPack32,
    A2R10G10B10UscaledPack32,
    A2R10G10B10SscaledPack32,
    A2R10G10B10UintPack32,
    A2R10G10B10SintPack32,
    A2B10G10R10UnormPack32,
    A2B10G10R10SnormPack32,
    A2B10G10R10UscaledPack32,
    A2B10G10R10SscaledPack32,
    A2B10G10R10UintPack32,
    A2B10G10R10SintPack32,
    R16Unorm,
    R16Snorm,
    R16Uscaled,
    R16Sscaled,
    R16Uint,
    R16Sint,
    R16Sfloat,
    R16G16Unorm,
    R16G16Snorm,
    R16G16Uscaled,
    R16G16Sscaled,
    R16G16Uint,
    R16G16Sint,
    R16G16Sfloat,
    R16G16B16Unorm,
    R16G16B16Snorm,
    R16G16B16Uscaled,
    R16G16B16Sscaled,
    R16G16B16Uint,
    R16G16B16Sint,
    R16G16B16Sfloat,
    R16G16B16A16Unorm,
    R16G16B16A16Snorm,
    R16G16B16A16Uscaled,
    R16G16B16A16Sscaled,
    R16G16B16A16Uint,
    R16G16B16A16Sint,
    R16G16B16A16Sfloat,
    R32Uint,
    R32Sint,
    R32Sfloat,
    R32G32Uint,
    R32G32Sint,
    R32G32Sfloat,
    R32G32B32Uint,
    R32G32B32Sint,
    R32G32B32Sfloat,
    R32G32B32A32Uint,
    R32G32B32A32Sint,
    R32G32B32A32Sfloat,
    R64Uint,
    R64Sint,
    R64Sfloat,
    R64G64Uint,
    R64G64Sint,
    R64G64Sfloat,
    R64G64B64Uint,
    R64G64B64Sint,
    R64G64B64Sfloat,
    R64G64B64A64Uint,
    R64G64B64A64Sint,
    R64G64B64A64Sfloat,
    B10G11R11UfloatPack32,
    E5B9G9R9UfloatPack32,
    D16Unorm,
    X8D24UnormPack32,
    D32Sfloat,
    S8Uint,
    D16UnormS8Uint,
    D24UnormS8Uint,
    D32SfloatS8Uint,
    Bc1RgbUnormBlock,
    Bc1RgbSrgbBlock,
    Bc1RgbaUnormBlock,
    Bc1RgbaSrgbBlock,
    Bc2UnormBlock,
    Bc2SrgbBlock,
    Bc3UnormBlock,
    Bc3SrgbBlock,
    Bc4UnormBlock,
    Bc4SnormBlock,
    Bc5UnormBlock,
    Bc5SnormBlock,
    Bc6HUfloatBlock,
    Bc6HSfloatBlock,
    Bc7UnormBlock,
    Bc7SrgbBlock,
    Etc2R8G8B8UnormBlock,
    Etc2R8G8B8SrgbBlock,
    Etc2R8G8B8A1UnormBlock,
    Etc2R8G8B8A1SrgbBlock,
    Etc2R8G8B8A8UnormBlock,
    Etc2R8G8B8A8SrgbBlock,
    EacR11UnormBlock,
    EacR11SnormBlock,
    EacR11G11UnormBlock,
    EacR11G11SnormBlock,
    Astc4x4UnormBlock,
    Astc4x4SrgbBlock,
    Astc5x4UnormBlock,
    Astc5x4SrgbBlock,
    Astc5x5UnormBlock,
    Astc5x5SrgbBlock,
    Astc6x5UnormBlock,
    Astc6x5SrgbBlock,
    Astc6x6UnormBlock,
    Astc6x6SrgbBlock,
    Astc8x5UnormBlock,
    Astc8x5SrgbBlock,
    Astc8x6UnormBlock,
    Astc8x6SrgbBlock,
    Astc8x8UnormBlock,
    Astc8x8SrgbBlock,
    Astc10x5UnormBlock,
    Astc10x5SrgbBlock,
    Astc10x6UnormBlock,
    Astc10x6SrgbBlock,
    Astc10x8UnormBlock,
    Astc10x8SrgbBlock,
    Astc10x10UnormBlock,
    Astc10x10SrgbBlock,
    Astc12x10UnormBlock,
    Astc12x10SrgbBlock,
    Astc12x12UnormBlock,
    Astc12x12SrgbBlock,
    G8B8G8R8422Unorm,
    B8G8R8G8422Unorm,
    G8B8R83Plane420Unorm,
    G8B8R82Plane420Unorm,
    G8B8R83Plane422Unorm,
    G8B8R82Plane422Unorm,
    G8B8R83Plane444Unorm,
    R10X6UnormPack16,
    R10X6G10X6Unorm2Pack16,
    R10X6G10X6B10X6A10X6Unorm4Pack16,
    G10X6B10X6G10X6R10X6422Unorm4Pack16,
    B10X6G10X6R10X6G10X6422Unorm4Pack16,
    G10X6B10X6R10X63Plane420Unorm3Pack16,
    G10X6B10X6R10X62Plane420Unorm3Pack16,
    G10X6B10X6R10X63Plane422Unorm3Pack16,
    G10X6B10X6R10X62Plane422Unorm3Pack16,
    G10X6B10X6R10X63Plane444Unorm3Pack16,
    R12X4UnormPack16,
    R12X4G12X4Unorm2Pack16,
    R12X4G12X4B12X4A12X4Unorm4Pack16,
    G12X4B12X4G12X4R12X4422Unorm4Pack16,
    B12X4G12X4R12X4G12X4422Unorm4Pack16,
    G12X4B12X4R12X43Plane420Unorm3Pack16,
    G12X4B12X4R12X42Plane420Unorm3Pack16,
    G12X4B12X4R12X43Plane422Unorm3Pack16,
    G12X4B12X4R12X42Plane422Unorm3Pack16,
    G12X4B12X4R12X43Plane444Unorm3Pack16,
    G16B16G16R16422Unorm,
    B16G16R16G16422Unorm,
    G16B16R163Plane420Unorm,
    G16B16R162Plane420Unorm,
    G16B16R163Plane422Unorm,
    G16B16R162Plane422Unorm,
    G16B16R163Plane444Unorm,
    Pvrtc12BppUnormBlockIMG,
    Pvrtc14BppUnormBlockIMG,
    Pvrtc22BppUnormBlockIMG,
    Pvrtc24BppUnormBlockIMG,
    Pvrtc12BppSrgbBlockIMG,
    Pvrtc14BppSrgbBlockIMG,
    Pvrtc22BppSrgbBlockIMG,
    Pvrtc24BppSrgbBlockIMG,
    G8B8G8R8422UnormKHR,
    B8G8R8G8422UnormKHR,
    G8B8R83Plane420UnormKHR,
    G8B8R82Plane420UnormKHR,
    G8B8R83Plane422UnormKHR,
    G8B8R82Plane422UnormKHR,
    G8B8R83Plane444UnormKHR,
    R10X6UnormPack16KHR,
    R10X6G10X6Unorm2Pack16KHR,
    R10X6G10X6B10X6A10X6Unorm4Pack16KHR,
    G10X6B10X6G10X6R10X6422Unorm4Pack16KHR,
    B10X6G10X6R10X6G10X6422Unorm4Pack16KHR,
    G10X6B10X6R10X63Plane420Unorm3Pack16KHR,
    G10X6B10X6R10X62Plane420Unorm3Pack16KHR,
    G10X6B10X6R10X63Plane422Unorm3Pack16KHR,
    G10X6B10X6R10X62Plane422Unorm3Pack16KHR,
    G10X6B10X6R10X63Plane444Unorm3Pack16KHR,
    R12X4UnormPack16KHR,
    R12X4G12X4Unorm2Pack16KHR,
    R12X4G12X4B12X4A12X4Unorm4Pack16KHR,
    G12X4B12X4G12X4R12X4422Unorm4Pack16KHR,
    B12X4G12X4R12X4G12X4422Unorm4Pack16KHR,
    G12X4B12X4R12X43Plane420Unorm3Pack16KHR,
    G12X4B12X4R12X42Plane420Unorm3Pack16KHR,
    G12X4B12X4R12X43Plane422Unorm3Pack16KHR,
    G12X4B12X4R12X42Plane422Unorm3Pack16KHR,
    G12X4B12X4R12X43Plane444Unorm3Pack16KHR,
    G16B16G16R16422UnormKHR,
    B16G16R16G16422UnormKHR,
    G16B16R163Plane420UnormKHR,
    G16B16R162Plane420UnormKHR,
    G16B16R163Plane422UnormKHR,
    G16B16R162Plane422UnormKHR,
    G16B16R163Plane444UnormKHR
  };

  enum class VertexInputRate
  {
    Vertex,
    Instance
  };

  enum class DescriptorType
  {
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformTexelBuffer,
    StorageTexelBuffer,
    UniformBuffer,
    StorageBuffer,
    UniformBufferDynamic,
    StorageBufferDynamic,
    InputAttachment,
    InlineUniformBlockEXT,
    AccelerationStructureNV
  };

  enum class ShaderStageFlags
  {
    Vertex = 0x00000001,
    TessellationControl = 0x00000002,
    TessellationEvaluation = 0x00000004,
    Geometry = 0x00000008,
    Fragment = 0x00000010,
    Compute = 0x00000020,
    AllGraphics = 0x0000001F,
    All = 0x7FFFFFFF,
    RaygenNV = 0x00000100,
    AnyHitNV = 0x00000200,
    ClosestHitNV = 0x00000400,
    MissNV = 0x00000800,
    IntersectionNV = 0x00001000,
    CallableNV = 0x00002000,
    TaskNV = 0x00000040,
    MeshNV = 0x00000080
  };

  inline bool operator&(ShaderStageFlags lhs, ShaderStageFlags rhs)
  {
    using T = std::underlying_type_t <ShaderStageFlags>;
    return static_cast<T>(lhs) & static_cast<T>(rhs);
  }

  enum class ImageLayout
  {
    Undefined,
    General,
    ColorAttachmentOptimal,
    DepthStencilAttachmentOptimal,
    DepthStencilReadOnlyOptimal,
    ShaderReadOnlyOptimal,
    TransferSrcOptimal,
    TransferDstOptimal,
    Preinitialized,
    DepthReadOnlyStencilAttachmentOptimal,
    DepthAttachmentStencilReadOnlyOptimal,
    PresentSrcKHR,
    SharedPresentKHR,
    ShadingRateOptimalNV,
    FragmentDensityMapOptimalEXT,
    DepthReadOnlyStencilAttachmentOptimalKHR,
    DepthAttachmentStencilReadOnlyOptimalKHR
  };

  struct VertexInputAttributeDescription
  {
    u32 location;
    u32 binding;
    VertexFormat format;
    u32 offset;
  };

  struct VertexInputBindingDescription
  {
    uint32_t binding;
    uint32_t stride;
    VertexInputRate inputRate;
  };

  struct DescriptorSetLayoutBinding
  {
    DescriptorSetLayoutBinding(u32 aBinding,
      DescriptorType aDescriptorType,
      ShaderStageFlags aStageFlags,
      size_t aSize,
      size_t aOffset)
      : mBinding{aBinding}
      , mDescriptorType{aDescriptorType}
      , mStageFlags{aStageFlags}
      , mType{Type::Buffer}
    {
      mBufferOrImage.mBuffer = Buffer{ aSize, aOffset };
    }

    DescriptorSetLayoutBinding(u32 aBinding,
      DescriptorType aDescriptorType,
      ShaderStageFlags aStageFlags,
      ImageLayout aLayout)
      : mBinding{ aBinding }
      , mDescriptorType{ aDescriptorType }
      , mStageFlags{ aStageFlags }
      , mType{ Type::Image }
    {
      mBufferOrImage.mLayout = aLayout;
    }

    enum class Type
    {
      Image,
      Buffer
    };

    u32 mBinding;
    DescriptorType mDescriptorType;
    ShaderStageFlags mStageFlags;
    Type mType;

    struct Buffer
    {
      size_t mSize;
      size_t mOffset;
    };

    union BufferOrImage
    {
      Buffer mBuffer;
      ImageLayout mLayout;
    } mBufferOrImage;
  };

  class ShaderDescriptions
  {
  public:
    ShaderDescriptions(size_t aNumberOfBindings = 2, size_t aNumberOfAttributes = 8)
    {
      mBindings.reserve(aNumberOfBindings);
      mAttributes.reserve(aNumberOfAttributes);
    }

    template <typename T>
    void AddAttribute(VertexFormat aFormat)
    {
      DebugObjection(mBindings.size() == 0,
        "Haven't added a Vertex binding yet, "
        "so we can't add attribute inputs.");

      VertexInputAttributeDescription toAdd;
      toAdd.binding = mBinding - 1;
      toAdd.location = mLocation;
      toAdd.format = aFormat;
      toAdd.offset = mVertexOffset;

      mAttributes.emplace_back(toAdd);

      ++mLocation;
      mVertexOffset += sizeof(T);
    }

    template <typename T>
    void AddBinding(VertexInputRate aDescription)
    {
      VertexInputBindingDescription toAdd;
      toAdd.binding = mBinding;
      toAdd.inputRate = aDescription;
      toAdd.stride = sizeof(T);

      mBindings.emplace_back(toAdd);

      ++mBinding;
      mVertexOffset = 0;
    }

    void AddDescriptor(DescriptorType aDescriptorType, ShaderStageFlags aStage, ImageLayout aLayout)
    {
      mDescriptorSetLayouts.emplace_back(mBufferBinding, aDescriptorType, aStage, aLayout);

      ++mBufferBinding;
    }

    void AddDescriptor(DescriptorType aDescriptorType, ShaderStageFlags aStage, size_t aBufferSize, size_t aBufferOffset = 0)
    {
      mDescriptorSetLayouts.emplace_back(mBufferBinding, aDescriptorType, aStage, aBufferSize, aBufferOffset);

      ++mBufferBinding;
    }

    u32 CountDescriptorsOfType(DescriptorType aType) const
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

    /////////////////////////////////
    // Getter 
    /////////////////////////////////
    VertexInputBindingDescription* BindingData()
    {
      return mBindings.data();
    }

    size_t BindingSize()
    {
      return mBindings.size();
    }

    std::vector<VertexInputBindingDescription>& Bindings()
    {
      return mBindings;
    }

    std::vector<VertexInputBindingDescription> const& Bindings() const
    {
      return mBindings;
    }

    std::vector<VertexInputAttributeDescription>& Attributes()
    {
      return mAttributes;
    }

    std::vector<VertexInputAttributeDescription> const& Attributes() const
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

      for (auto& line : mLines)
      {
        w.write(line);
        w.write("\n");
      }

      return w.str();
    }


    std::vector<std::string> const& Lines() const
    {
      return mLines;
    }

    std::vector<DescriptorSetLayoutBinding>& DescriptorSetLayouts()
    {
      return mDescriptorSetLayouts;
    }

    std::vector<DescriptorSetLayoutBinding> const& DescriptorSetLayouts() const
    {
      return mDescriptorSetLayouts;
    }

  private:
    // Vertex Input Data
    std::vector<VertexInputBindingDescription> mBindings;
    std::vector<VertexInputAttributeDescription> mAttributes;
    std::vector<std::string> mLines;
    u32 mBinding = 0;
    u32 mVertexOffset = 0;
    u32 mLocation = 0;
    u32 mConstant = 0;

    // Buffer Data
    std::vector<DescriptorSetLayoutBinding> mDescriptorSetLayouts;
    u32 mBufferBinding = 0;
  };
}

#endif
