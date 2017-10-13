#include "YTE/Graphics/Renderer.hpp"
#include "YTE/Graphics/Vulkan/VkPrimitives.hpp"


namespace YTE
{
  struct RenderedSurface;

  struct InstantiatedMeshRendererData
  {
    InstantiatedMeshRendererData(Renderer *aRenderer,
      Mesh *aMesh,
      Window *aWindow)
      : mRenderer(aRenderer),
      mMesh(aMesh),
      mWindow(aWindow)
    {

    }

    ~InstantiatedMeshRendererData()
    {
      for (auto &subMeshData : mSubmeshPipelineData)
      {
        mRenderer->RemoveMeshId(mWindow, subMeshData.mId);
      }
    }

    void UpdateUniformBuffer(InstantiatedMesh &aModel);

    struct SubmeshPipelineData
    {
      SubmeshPipelineData()
      {

      }

      std::shared_ptr<vkhlf::PipelineLayout> mPipelineLayout;
      std::shared_ptr<vkhlf::DescriptorSet> mDescriptorSet;

      u64 mId;
    };

    RenderedSurface *mSurface;
    std::shared_ptr<vkhlf::Buffer> mUBOModel;

    std::vector<SubmeshPipelineData> mSubmeshPipelineData;

    Renderer *mRenderer;
    Mesh *mMesh;
    Window *mWindow;
  };

  class VkRenderer : public Renderer
  {

    using TextureMap = std::unordered_map<std::string,
      std::unique_ptr<Texture>>;

    using MeshMap = std::unordered_map<std::string,
      std::unique_ptr<Mesh>>;

    using AllocatorMap = std::unordered_map<std::string,
      std::shared_ptr<vkhlf::DeviceMemoryAllocator>>;
  public:

    ~VkRenderer();

    VkRenderer(Engine *aEngine);

    Texture *AddTexture(Window *aWindow,
      const char *aTexture) override;

    void UpdateViewBuffer(Window *aWindow,
      UBOView &aView) override;

    virtual void UpdateModelTransformation(Model *aModel) override;

    Texture* AddTexture(RenderedSurface *aSurface,
      const char *aTextureName);

    void AddDescriptorSet(RenderedSurface *aSurface,
      InstantiatedMesh *aModel,
      Mesh::SubMesh *aSubMesh,
      InstantiatedMeshRendererData::SubmeshPipelineData *aSubmeshPipelineData);

    std::shared_ptr<vkhlf::Pipeline> AddPipeline(RenderedSurface *aSurface,
      InstantiatedMesh *aModel,
      Mesh::SubMesh *aSubMesh,
      InstantiatedMeshRendererData::SubmeshPipelineData *aSubmeshPipelineData);

    Mesh* AddMesh(RenderedSurface *aSurface,
      std::string &aFilename);

    void RemoveMeshId(Window *aWindow, u64 aId) override;

    std::unique_ptr<InstantiatedMesh> AddModel(Window *aWindow,
      std::string &aMeshFile) override;

    glm::vec4 GetClearColor(Window *aWindow) override;
    void SetClearColor(Window *aWindow, const glm::vec4 &aColor) override;

    std::unordered_map<vkhlf::Device*, AllocatorMap>& GetAllocators()
    {
      return mAllocators;
    }

  private:

    std::unordered_map<vkhlf::Device*, AllocatorMap> mAllocators;
    std::unordered_map<vkhlf::Device*, MeshMap> mMeshes;
    std::unordered_map<vkhlf::Device*, TextureMap> mTextures;
    std::shared_ptr<vkhlf::DebugReportCallback> mDebugReportCallback;
    std::shared_ptr<vkhlf::Instance> mInstance;

    std::unordered_map<Window*, std::unique_ptr<RenderedSurface>> mSurfaces;
    u64 mMeshIdCounter = 0;

    Engine *mEngine;
  };


  struct RenderedSurface;


  struct TextureRendererData
  {
    std::shared_ptr<vkhlf::Sampler> mSampler;
    std::shared_ptr<vkhlf::ImageView> mView;
    std::shared_ptr<vkhlf::Image> mImage;

    vk::ImageLayout mImageLayout;
    vk::DeviceMemory mDeviceMemory;
    vk::DescriptorImageInfo mDescriptor;
  };

  struct MeshRendererData
  {
    std::shared_ptr<vkhlf::Buffer> mVertexBuffer;
    std::shared_ptr<vkhlf::Buffer> mIndexBuffer;
    std::shared_ptr<vkhlf::Buffer> mUBOMaterial;
  };

}
