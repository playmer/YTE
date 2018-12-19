///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#pragma once

#ifndef YTE_Graphics_Vulkan_VkRenderer_hpp
#define YTE_Graphics_Vulkan_VkRenderer_hpp

#include <unordered_map>

#include "YTE/Graphics/GPUBuffer.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Graphics/Vulkan/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "YTE/Graphics/Vulkan/VkCommandBufferBuffer.hpp"

namespace YTE
{

  struct VkUBOUpdates
  {
    VkUBOUpdates(VkRenderer* aRenderer)
      : mRenderer{aRenderer}
    {

    }

    struct VkUBOReference
    {
      VkUBOReference(std::shared_ptr<vkhlf::Buffer> const& aBuffer,
                     size_t aBufferOffset,
                     size_t aSize);

      std::shared_ptr<vkhlf::Buffer> mBuffer;
      size_t mBufferOffset;
      size_t mSize;
    };

    void Add(std::shared_ptr<vkhlf::Buffer> const& aBuffer, u8 const* aData, size_t aSize, size_t aOffset);

    template <typename tType>
    void Add(std::shared_ptr<vkhlf::Buffer> const& aBuffer, tType const& aData, size_t aNumber = 1, size_t aOffset = 0)
    {
      Add(aBuffer, reinterpret_cast<u8 const*>(&aData), sizeof(tType) * aNumber, aOffset);
    }

    void Update(std::shared_ptr<vkhlf::CommandBuffer>& aBuffer);

    std::vector<u8> mData;
    std::vector<VkUBOReference> mReferences;
    std::shared_ptr<vkhlf::Buffer> mMappingBuffer;
    VkRenderer* mRenderer;
    //size_t mBytesLastUsed = 0;
  };

  class VkRenderer : public Renderer
  {
  public:
    YTEDeclareType(VkRenderer);

    VkRenderer(Engine *aEngine);
    ~VkRenderer() override;

    void DeregisterWindowFromDraw(Window *aWindow) override;
    void RegisterWindowForDraw(Window *aWindow) override;

    std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, std::string &aMeshFile) override;
    std::unique_ptr<InstantiatedModel> CreateModel(GraphicsView *aView, Mesh *aMesh) override;
    void DestroyMeshAndModel(GraphicsView *aView, InstantiatedModel *aModel) override;
    std::unique_ptr<InstantiatedLight> CreateLight(GraphicsView *aView) override;
    std::unique_ptr<InstantiatedInfluenceMap> CreateWaterInfluenceMap(GraphicsView *aView) override;

    VkTexture* CreateTexture(std::string &aFilename, vk::ImageViewType aType);
    VkTexture* CreateTexture(std::string aName,
                             std::vector<u8> aData,
                             TextureLayout aType,
                             u32 aWidth,
                             u32 aHeight,
                             u32 aMipLevels,
                             u32 aLayerCount,
                             vk::ImageViewType aVulkanType);


    Texture* CreateTexture(std::string &aFilename, TextureType aType) override;
    Texture* CreateTexture(std::string aName,
                           std::vector<u8> aData,
                           TextureLayout aLayout,
                           u32 aWidth,
                           u32 aHeight,
                           u32 aMipLevels,
                           u32 aLayerCount,
                           TextureType aType) override;
        
    void UpdateWindowViewBuffer(GraphicsView *aView, UBOs::View &aUBOView) override;
    void UpdateWindowIlluminationBuffer(GraphicsView *aView, UBOs::Illumination &aIllumination) override;

    VkMesh* CreateMesh(std::string &aFilename);
    Mesh* CreateSimpleMesh(std::string &aName,
                           std::vector<Submesh> &aSubmeshes,
		                       bool aForceUpdate = false) override;

    void ResetView(GraphicsView *aView);


    /////////////////////////////////
    // Events
    /////////////////////////////////
    void GraphicsDataUpdate(LogicUpdate *aEvent) override;
    void FrameUpdate(LogicUpdate *aEvent) override;
    void PresentFrame(LogicUpdate *aEvent) override;

    void SetLights(bool aOnOrOff);
    void RegisterView(GraphicsView *aView) override;
    void RegisterView(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination) override;
    void SetViewDrawingType(GraphicsView *aView, DrawerTypes aDrawerType, DrawerTypeCombination aCombination) override;
    void SetViewCombinationType(GraphicsView *aView, DrawerTypeCombination aCombination) override;
    void DeregisterView(GraphicsView *aView) override;
    void ViewOrderChanged(GraphicsView *aView, float aNewOrder) override;

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    glm::vec4 GetClearColor(GraphicsView *aView);
    void SetClearColor(GraphicsView *aView, const glm::vec4 &aColor) override;
    VkRenderedSurface* GetSurface(Window *aWindow);

    VkWaterInfluenceMapManager* GetAllWaterInfluenceMaps(GraphicsView *aView);

    Engine* GetEngine() const
    {
      return mEngine;
    }

    std::unordered_map<Window*, std::unique_ptr<VkRenderedSurface>>& GetSurfaces()
    {
      return mSurfaces;
    }

    VkInternals* GetVkInternals() const
    {
      return mVulkanInternals.get();
    }

    GPUAllocator* MakeAllocator(std::string const& aAllocatorType, size_t aBlockSize) override;

    std::shared_ptr<vkhlf::Device> mDevice;
    //std::unordered_map<std::string, std::shared_ptr<vkhlf::DeviceMemoryAllocator>> mAllocators;
    std::unordered_map<std::string, std::unique_ptr<VkTexture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<VkMesh>> mMeshes;
    std::shared_ptr<vkhlf::Queue> mGraphicsQueue;
    std::shared_ptr<vkhlf::CommandPool> mCommandPool;

    VkUBOUpdates mUBOUpdates;
  private:
    bool mDataUpdateRequired = false;
    // create a command pool for command buffer allocation
    std::unique_ptr<VkCBOB<3, false>> mGraphicsDataUpdateCBOB;

    std::unique_ptr<VkInternals> mVulkanInternals;
    std::unordered_map<Window*, std::unique_ptr<VkRenderedSurface>> mSurfaces;
    Engine *mEngine;

  };

  struct VkUBOData
  {
    std::shared_ptr<vkhlf::Buffer> mBuffer;
    VkRenderer *mRenderer;
  };

  class VkUBO : public GPUBufferBase
  {
  public:
    VkUBO(size_t aSize)
      : GPUBufferBase{ aSize }
    {

    }

    void Update(u8 const* aPointer, size_t aBytes, size_t aOffset) override
    {
      auto self = mData.Get<VkUBOData>();

      self->mRenderer->mUBOUpdates.Add(self->mBuffer, aPointer, aBytes, aOffset);
    }

    std::shared_ptr<vkhlf::Buffer>& GetBuffer()
    {
      auto self = mData.Get<VkUBOData>();

      return self->mBuffer;
    }
  };

  template <typename tType>
  std::shared_ptr<vkhlf::Buffer>& GetBuffer(GPUBuffer<tType>& aBuffer)
  {
    return static_cast<VkUBO*>(&aBuffer.GetBase())->GetBuffer();
  }

  struct VkGPUAllocatorData
  {
    VkGPUAllocatorData(std::shared_ptr<vkhlf::DeviceMemoryAllocator>& aAllocator,
                       std::shared_ptr<vkhlf::Device>& aDevice,
                       VkRenderer* aRenderer)
      : mAllocator{aAllocator}
      , mDevice{ aDevice }
      , mRenderer{ aRenderer }
    {

    }

    std::shared_ptr<vkhlf::DeviceMemoryAllocator> mAllocator;
    std::shared_ptr<vkhlf::Device> mDevice;
    VkRenderer* mRenderer;
  };


  class VkGPUAllocator : public GPUAllocator
  {
  public:
    VkGPUAllocator(size_t aBlockSize, VkRenderer* aRenderer);
    std::unique_ptr<GPUBufferBase> CreateBufferInternal(size_t aSize,
                                                        GPUAllocation::BufferUsage aUse, 
                                                        GPUAllocation::MemoryProperty aProperties) override;
  };

  inline std::shared_ptr<vkhlf::DeviceMemoryAllocator>& GetAllocator(GPUAllocator* aBuffer)
  {
    return aBuffer->GetData().Get<VkGPUAllocatorData>()->mAllocator;
  }
}


#endif
