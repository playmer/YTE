///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Vulkan
///////////////////

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Vulkan/VkRenderToScreen.hpp"
#include "YTE/Graphics/Vulkan/VkInternals.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkShaderCompiler.hpp"
#include "YTE/Graphics/Vulkan/VkTexture.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineType(VkRenderToScreen)
  {
    RegisterType<VkRenderToScreen>();
    TypeBuilder<VkRenderToScreen> builder;
  }


  VkRenderToScreen::VkRenderToScreen(Window *aWindow,
                                     VkRenderer *aRenderer,
                                     VkRenderedSurface *aSurface,
                                     vk::Format aColorFormat,
                                     vk::Format aDepthFormat,
                                     std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                     std::string aShaderSetName)
    : mWindow(aWindow)
    , mRenderer(aRenderer)
    , mSurface(aSurface)
    , mColorFormat(aColorFormat)
    , mDepthFormat(aDepthFormat)
    , mVulkanSurface(aVulkanSurface)
    , mShaderSetName(aShaderSetName)
    , mScreenQuad(nullptr)
    , mScreenShader(nullptr)
  {
    LoadToVulkan();

    mCBOB = std::make_unique<VkCBOB<3, true>>(mSurface->GetCommandPool());
    mCBEB = std::make_unique<VkCBEB<3>>(mSurface->GetDevice());

    CreateRenderPass();
    //ReloadQuad();
    //ReloadShaders(false);
  }



  VkRenderToScreen::~VkRenderToScreen()
  {
    mFrameBufferSwapChain.reset();
    mRenderPass.reset();
    mScreenQuad.reset();
    mScreenShader.reset();
  }


  void VkRenderToScreen::Resize(vk::Extent2D& aExtent)
  {
    CreateSwapChain(aExtent);
    //ReloadQuad();
    //ReloadShaders(false);
  }

  

  bool RenderTargetSorter(VkRenderTarget::RenderTargetData* a,
                          VkRenderTarget::RenderTargetData* b)
  {
    if (a->mOrder < b->mOrder)
    {
      return true;
    }
    return false;
  }



  void VkRenderToScreen::ResetRenderTargets(std::vector<VkRenderTarget*>& aRTs)
  {
    mRenderTargetData.clear();
    for (int i = 0; i < aRTs.size(); ++i)
    {
      mRenderTargetData.push_back((aRTs[i]->GetRenderTargetData()));
    }
    std::sort(mRenderTargetData.begin(),
              mRenderTargetData.end(),
              RenderTargetSorter);

    if (mScreenQuad == nullptr || mScreenShader == nullptr)
    {
      LoadQuad();
      LoadShaders();
    }
    else
    {
      mScreenQuad->Resize();
      //ReloadQuad();
      //ReloadShaders();
    }
  }



  void VkRenderToScreen::SetRenderTargets(std::vector<VkRenderTarget*>& aRTs)
  {
    mRenderTargetData.clear();
    for (int i = 0; i < aRTs.size(); ++i)
    {
      VkRenderTarget::RenderTargetData* v = aRTs[i]->GetRenderTargetData();
      mRenderTargetData.push_back(v);
    }
    std::sort(mRenderTargetData.begin(),
              mRenderTargetData.end(),
              RenderTargetSorter);

    if (mScreenQuad == nullptr || mScreenShader == nullptr)
    {
      LoadQuad();
      LoadShaders();
    }
    else
    {
      ReloadQuad();
      ReloadShaders(true);
    }
  }



  void VkRenderToScreen::FrameUpdate()
  {
    mFrameBufferSwapChain->acquireNextFrame();
  }



  const vk::Extent2D& VkRenderToScreen::GetExtent()
  {
    return mFrameBufferSwapChain->getExtent();
  }



  void VkRenderToScreen::MoveToNextEvent()
  {
    mCBEB->NextEvent();
  }



  void VkRenderToScreen::ExecuteSecondaryEvent(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    auto& e = mCBEB->GetCurrentEvent();
    aCBO->setEvent(e, vk::PipelineStageFlagBits::eBottomOfPipe);
  }



  void VkRenderToScreen::ExecuteCommands(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    aCBO->executeCommands(mCBOB->GetCurrentCBO());
  }

  void VkRenderToScreen::RenderFull(const vk::Extent2D& aExtent)
  {
    mCBOB->NextCommandBuffer();
    auto cbo = mCBOB->GetCurrentCBO();
    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mRenderPass);

    auto width = static_cast<float>(aExtent.width);
    auto height = static_cast<float>(aExtent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    cbo->setViewport(0, viewport);
    cbo->setLineWidth(1.0f);

    vk::Rect2D scissor{ { 0, 0 }, aExtent };
    cbo->setScissor(0, scissor);

    Render(cbo);
    cbo->end();
  }



  void VkRenderToScreen::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    mScreenShader->Bind(aCBO);
    mScreenQuad->Bind(aCBO);
    mScreenQuad->Render(aCBO);
  }

  bool VkRenderToScreen::PresentFrame(std::shared_ptr<vkhlf::Queue>& aGraphicsQueue,
                                      std::shared_ptr<vkhlf::Semaphore>& aRenderCompleteSemaphore)
  {
    try
    {
      mFrameBufferSwapChain->present(aGraphicsQueue, aRenderCompleteSemaphore);
    }
    catch (...)
    {
      return false;
    }

    return true;
  }



  void VkRenderToScreen::CreateSwapChain(vk::Extent2D& aExtent)
  {
    mWindow->SetExtent(aExtent.width, aExtent.height);

    DebugObjection((0 > aExtent.width) || (0 > aExtent.height),
                   "Resizing to a negative x or y direction is not possible");

    // TODO (Josh): According to vkhlf, you have to do this little dance,
    //              unsure why, should find out.
    mFrameBufferSwapChain.reset();
    mFrameBufferSwapChain = std::make_unique<vkhlf::FramebufferSwapchain>(mSurface->GetDevice(),
                                                                          mVulkanSurface,
                                                                          mColorFormat,
                                                                          mDepthFormat,
                                                                          mRenderPass);

    DebugObjection(mFrameBufferSwapChain->getExtent() != aExtent,
                   "Swap chain extent did not update with resize");
  }



  void VkRenderToScreen::CreateRenderPass()
  {
    // Attachment Descriptions
    vk::AttachmentDescription colorAttachment{ {},
                                               mColorFormat,
                                               vk::SampleCountFlagBits::e1,
                                               //vk::AttachmentLoadOp::eLoad,
                                               vk::AttachmentLoadOp::eClear,
                                               vk::AttachmentStoreOp::eStore, // color
                                               vk::AttachmentLoadOp::eDontCare,
                                               vk::AttachmentStoreOp::eDontCare, // stencil
                                               vk::ImageLayout::eUndefined,
                                               vk::ImageLayout::ePresentSrcKHR };

    vk::AttachmentDescription depthAttachment{ {},
                                               mDepthFormat,
                                               vk::SampleCountFlagBits::e1,
                                               vk::AttachmentLoadOp::eClear,
                                               vk::AttachmentStoreOp::eStore, // depth
                                               vk::AttachmentLoadOp::eDontCare,
                                               vk::AttachmentStoreOp::eDontCare, // stencil
                                               vk::ImageLayout::eUndefined,
                                               vk::ImageLayout::eDepthStencilAttachmentOptimal };

    std::array<vk::AttachmentDescription, 2> attachmentDescriptions{ colorAttachment,
                                                                     depthAttachment };

    // Subpass Description
    vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass{ {},
                                    vk::PipelineBindPoint::eGraphics,
                                    0,
                                    nullptr,
                                    1,
                                    &colorReference,
                                    nullptr,
                                    &depthReference,
                                    0,
                                    nullptr };

    std::array<vk::SubpassDependency, 2> subpassDependencies;

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commands executed outside of the actual renderpass)
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                           vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // Transition from initial to final
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                           vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    mRenderPass = mSurface->GetDevice()->createRenderPass(attachmentDescriptions, subpass, subpassDependencies);

  }


  void VkRenderToScreen::LoadToVulkan()
  {
    if (mScreenQuad)
    {
      mScreenQuad->LoadToVulkan();
    }
  }

  void VkRenderToScreen::ReloadQuad()
  {
    mScreenQuad.reset();
    mScreenQuad = std::make_unique<ScreenQuad>(this);

    LoadToVulkan();
  }

  void VkRenderToScreen::ReloadShaders(bool aFromSet)
  {
    UnusedArguments(aFromSet);
    mScreenShader.reset();
    mScreenShader = std::make_unique<ScreenShader>(this, mScreenQuad.get(), mShaderSetName, false);
    LoadToVulkan();
  }

  void VkRenderToScreen::LoadQuad()
  {
    ReloadQuad();
  }

  void VkRenderToScreen::LoadShaders()
  {
    ReloadShaders();
  }


  // /////////////////////////////////////////////////////////////////////
  // Screen Quad
  // /////////////////////////////////////////////////////////////////////

  VkRenderToScreen::ScreenQuad::ScreenQuad(VkRenderToScreen* aParent)
    : mParent(aParent)
  {
    mIndices.reserve(6);
    mVertices.reserve(4);
    Create();
  }



  VkRenderToScreen::ScreenQuad::~ScreenQuad()
  {
    Destroy();
  }



  void VkRenderToScreen::ScreenQuad::Create()
  {
    // Mesh
    Vertex v1, v2, v3, v4;

    // note actual positions are in shader (top left of screen is -1,-1 and middle of screen is 0,0 and bottom right of screen is 1,1

    v1.mPosition = glm::vec3(1.0f, 1.0f, 0.0f);
    v1.mNormal = glm::vec3(0.0f, 0.0f, 1.0f);
    v1.mTextureCoordinates = glm::vec3(1.0f, 1.0f, 0.0f);
    v1.mColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    v2.mPosition = glm::vec3(0.0f, 1.0f, 0.0f);
    v2.mNormal = glm::vec3(0.0f, 0.0f, 1.0f);
    v2.mTextureCoordinates = glm::vec3(0.0f, 1.0f, 0.0f);
    v2.mColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    v3.mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    v3.mNormal = glm::vec3(0.0f, 0.0f, 1.0f);
    v3.mTextureCoordinates = glm::vec3(0.0f, 0.0f, 0.0f);
    v3.mColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    v4.mPosition = glm::vec3(1.0f, 0.0f, 0.0f);
    v4.mNormal = glm::vec3(0.0f, 0.0f, 1.0f);
    v4.mTextureCoordinates = glm::vec3(1.0f, 0.0f, 0.0f);
    v4.mColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

    mIndices.push_back(2);
    mIndices.push_back(1);
    mIndices.push_back(0);
    mIndices.push_back(0);
    mIndices.push_back(3);
    mIndices.push_back(2);
    mIndexCount = 6;

    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v4);

    auto allocator = mParent->mSurface->GetRenderer()->GetAllocator(AllocatorTypes::UniformBufferObject);

    mVertexBuffer = allocator->CreateBuffer<Vertex>(mVertices.size(),
                                                    GPUAllocation::BufferUsage::TransferDst | 
                                                    GPUAllocation::BufferUsage::VertexBuffer,
                                                    GPUAllocation::MemoryProperty::DeviceLocal);

    mIndexBuffer = allocator->CreateBuffer<u32>(mIndices.size(),
                                                GPUAllocation::BufferUsage::TransferDst | 
                                                GPUAllocation::BufferUsage::IndexBuffer,
                                                GPUAllocation::MemoryProperty::DeviceLocal);


    Resize();
  }



  void VkRenderToScreen::ScreenQuad::Resize()
  {
    auto device = mParent->mSurface->GetDevice();
    size_t samplers{ 0 };
    std::vector<std::string> samplerTypes;
    mSamplers.clear();

    for (int i = 0; i < mParent->mRenderTargetData.size(); ++i)
    {
      std::pair<std::string, DrawerTypeCombination> pair;
      pair.first = mParent->mRenderTargetData[i]->mName;
      pair.second = mParent->mRenderTargetData[i]->mCombinationType;
      samplerTypes.push_back(pair.first);
      mSamplers.push_back(pair);
      samplers++;
    }

    std::vector<vkhlf::DescriptorSetLayoutBinding> dslbs;
    //u32 uniformBuffers{ 0 };
    u32 binding{ 0 };
    VkShaderDescriptions descriptions;

    // view buffer
    //  dslbs.emplace_back(binding, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, nullptr);
    //  descriptions.AddPreludeLine(fmt::format("#define UBO_VIEW_BINDING {}", binding));
    //  ++uniformBuffers;

    for (size_t i = 0; i < samplers; ++i)
    {
      dslbs.emplace_back(binding,
                         vk::DescriptorType::eCombinedImageSampler,
                         vk::ShaderStageFlagBits::eFragment,
                         nullptr);
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", samplerTypes[i], binding++));
    }


    descriptions.AddBinding<Vertex>(vk::VertexInputRate::eVertex);
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mPosition;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mTextureCoordinates;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mNormal;
    descriptions.AddAttribute<glm::vec4>(vk::Format::eR32G32B32A32Sfloat); //glm::vec3 mColor;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mTangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mBinormal;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec3 mBitangent;
    descriptions.AddAttribute<glm::vec3>(vk::Format::eR32G32B32Sfloat);    //glm::vec4 mBoneWeights;
    descriptions.AddAttribute<glm::vec2>(vk::Format::eR32G32Sfloat);       //glm::vec2 mBoneWeights2;
    descriptions.AddAttribute<glm::ivec3>(vk::Format::eR32G32B32Sint);     //glm::ivec4 mBoneIDs;
    descriptions.AddAttribute<glm::ivec2>(vk::Format::eR32G32Sint);        //glm::ivec4 mBoneIDs;

    std::string defines;

    std::vector<vk::DescriptorPoolSize> descriptorTypes;
    //descriptorTypes.emplace_back(vk::DescriptorType::eUniformBuffer, uniformBuffers);

    if (0 != samplers)
    {
      descriptorTypes.emplace_back(vk::DescriptorType::eCombinedImageSampler, static_cast<u32>(samplers));
    }

    mDescriptorSetLayout = device->createDescriptorSetLayout(dslbs);

    auto pipelineLayout = device->createPipelineLayout(mDescriptorSetLayout, nullptr);

    if (descriptorTypes.size() == 0)
    {
      return;
    }

    auto pool = device->createDescriptorPool({}, 1, descriptorTypes);

    mDescriptorSet = device->allocateDescriptorSet(pool, mDescriptorSetLayout);

    std::vector<vkhlf::WriteDescriptorSet> wdss;

    constexpr auto unibuf = vk::DescriptorType::eUniformBuffer;
    auto &ds = mDescriptorSet;

    // We must reset binding to 0, as the previous value was for setting up the layout binding 
    // and we need a fresh 0ing for sake of getting our bindings accurate. 
    binding = 0;

    // add uniform buffers
    // view buffer
    //vkhlf::DescriptorBufferInfo uboView{mParent->mSurface->GetUBOViewBuffer(View), 0, sizeof(UBOs::View)};
    // wdss.emplace_back(ds, binding++, 0, 1, unibuf, nullptr, uboView);



    // Add Texture Samplers
    auto addTS = [&wdss, &binding, &ds](size_t aAttachmentIndex,
                                        VkRenderTarget::RenderTargetData *aData, 
                                        vkhlf::DescriptorImageInfo &aImageInfo)
    {
      constexpr auto imgsam = vk::DescriptorType::eCombinedImageSampler;

      if (nullptr == aData)
      {
        return;
      }

      aImageInfo.sampler = aData->mSampler;
      aImageInfo.imageView = aData->mAttachments[aAttachmentIndex].mImageView;
      wdss.emplace_back(ds, binding++, 0, 1, imgsam, aImageInfo, nullptr);
    };

    for (u32 i = 0; i < mParent->mRenderTargetData.size(); ++i)
    {
      auto &data = mParent->mRenderTargetData[i];
      for (size_t k = 0; k < data->mColorAttachments.size(); ++k)
      {
        vkhlf::DescriptorImageInfo dTexInfo{ nullptr, nullptr, vk::ImageLayout::eShaderReadOnlyOptimal };
        addTS(data->mColorAttachments[k], data, dTexInfo);
      }
    }

    device->updateDescriptorSets(wdss, nullptr);


    mShaderData.mDefines = defines;
    mShaderData.mPipelineLayout = pipelineLayout;
    mShaderData.mDescriptions = descriptions;
  }



  void VkRenderToScreen::ScreenQuad::Destroy()
  {
    mVertexBuffer.reset();
    mIndexBuffer.reset();
    mDescriptorSetLayout.reset();
    mDescriptorSet.reset();
    mVertices.clear();
    mIndices.clear();
    mShaderData.mPipelineLayout.reset();
  }

  void VkRenderToScreen::ScreenQuad::LoadToVulkan()
  {
    mVertexBuffer.Update(mVertices.data(), mVertices.size());
    mIndexBuffer.Update(mIndices.data(), mIndices.size());
  }

  void VkRenderToScreen::ScreenQuad::Bind(std::shared_ptr<vkhlf::CommandBuffer> aCBO)
  {
    aCBO->bindVertexBuffer(0, GetBuffer(mVertexBuffer), 0);
    aCBO->bindIndexBuffer(GetBuffer(mIndexBuffer), 0, vk::IndexType::eUint32);

    aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                             mShaderData.mPipelineLayout,
                             0,
                             mDescriptorSet,
                             nullptr);
  }

  void VkRenderToScreen::ScreenQuad::Render(std::shared_ptr<vkhlf::CommandBuffer> aCBO)
  {
    aCBO->drawIndexed(mIndexCount, 1, 0, 0, 0);
  }


  // /////////////////////////////////////////////////////////////////////
  // Screen Shader
  // /////////////////////////////////////////////////////////////////////

  VkRenderToScreen::ScreenShader::ScreenShader(VkRenderToScreen* aParent,
                                               ScreenQuad* aSibling,
                                               std::string &aShaderSetName,
                                               bool aReload)
    : mParent(aParent)
    , mSibling(aSibling)
  {
    Create(aShaderSetName, aReload);
  }



  VkRenderToScreen::ScreenShader::~ScreenShader()
  {
    Destroy();
  }



  void VkRenderToScreen::ScreenShader::Create(std::string& aShaderSetName, bool aReload)
  {
    auto modelData = mSibling->GetShaderData();

    auto device = mParent->mSurface->GetDevice();

    std::string vertex = aShaderSetName + ".vert";
    std::string fragment = aShaderSetName + ".frag";

    auto vertexFile = Path::GetShaderPath(Path::GetEnginePath(), vertex.c_str());

    if (false == std::filesystem::exists(vertexFile))
    {
      auto engine = mParent->mSurface->GetRenderer()->GetEngine();
      engine->Log(LogType::Information, fmt::format("Could not find the vertex file: {}", vertexFile));
      return;
    }

    std::string fragmentFile = GenerateFragmentShader();

    auto lines = modelData.mDescriptions.GetLines();
    auto vertexData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eVertex, vertexFile, lines, false);
    auto fragmentData = CompileGLSLToSPIRV(vk::ShaderStageFlagBits::eFragment, fragmentFile, lines, true);

    if (false == vertexData.mValid || false == fragmentData.mValid)
    {
      auto engine = mParent->mSurface->GetRenderer()->GetEngine();

      auto str = fmt::format("Vertex Shader named {}:\n {}\n-----------------\nFragment Shader named {}:\n {}",
                             vertex,
                             vertexData.mReason,
                             //fragmentFile,
                             "Generated Fragment Shader",
                             fragmentData.mReason);

      //std::cout << str;
      if (aReload)
      {
        engine->Log(LogType::Error, fmt::format(
          "\t-> {} Failed to Reload!\n############################################################\n",
          aShaderSetName));
      }
      else 
      {
        engine->Log(LogType::Error, fmt::format(
          "Shader: {} Failed to Load!\n############################################################\n",
          aShaderSetName));
      }


      engine->Log(LogType::Information, fmt::format(
        "Generated Fragement Shader : \n{}\n------------------------------------------\n",
        fragmentFile));

      engine->Log(LogType::Information, fmt::format(
        "Errors Follow:\n{}\n############################################################\n",
        str));

      DebugObjection(true,
                     fmt::format("Shader {} failed to compile and had no previously compiled shader to use.\n"
                                 "Compilation Message:\n"
                                 "{}",
                                 aShaderSetName, 
                                 str).c_str());
      return;
    }

    auto vertexModule = device->createShaderModule(vertexData.mData);
    auto fragmentModule = device->createShaderModule(fragmentData.mData);

    // Initialize Pipeline
    std::shared_ptr<vkhlf::PipelineCache> pipelineCache = device->createPipelineCache(0, nullptr);
    
    vkhlf::PipelineShaderStageCreateInfo vertexStage(vk::ShaderStageFlagBits::eVertex, vertexModule, "main");
    vkhlf::PipelineShaderStageCreateInfo fragmentStage(vk::ShaderStageFlagBits::eFragment, fragmentModule, "main");
    
    vkhlf::PipelineVertexInputStateCreateInfo vertexInput(modelData.mDescriptions.Bindings(),
                                                          modelData.mDescriptions.Attributes());

    vk::PipelineInputAssemblyStateCreateInfo assembly({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);

    vkhlf::PipelineViewportStateCreateInfo viewport({ {} }, { {} });

    vk::PipelineRasterizationStateCreateInfo rasterization({},
                                                           false,
                                                           false,
                                                           vk::PolygonMode::eFill,
                                                           vk::CullModeFlagBits::eBack,
                                                           vk::FrontFace::eCounterClockwise, 
                                                           false, 
                                                           0.0f, 
                                                           0.0f, 
                                                           0.0f, 
                                                           1.0f);

    vkhlf::PipelineMultisampleStateCreateInfo multiSample(vk::SampleCountFlagBits::e1, false, 0.0f, nullptr, false, false);

    vk::StencilOpState stencilOpState(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways, 0, 0, 0);

    vk::PipelineDepthStencilStateCreateInfo enableDepthStencil({},
                                                               true,
                                                               true,
                                                               vk::CompareOp::eLessOrEqual,
                                                               false,
                                                               false, 
                                                               stencilOpState,
                                                               stencilOpState,
                                                               0.0f, 
                                                               0.0f);

    vk::PipelineColorBlendAttachmentState noColorBlendAttachment(false,                                 // enable
                                                                 vk::BlendFactor::eSrcColor,            // SrcColorBlendFactor
                                                                 vk::BlendFactor::eOne,                 // DstColorBlendFactor
                                                                 vk::BlendOp::eAdd,                     // ColorBlendOp
                                                                 vk::BlendFactor::eOne,                 // SrcAlphaBlendFactor
                                                                 vk::BlendFactor::eOneMinusSrcAlpha,    // DstAlphaBlendFactor
                                                                 vk::BlendOp::eAdd,                     // AlphaBlendOp
                                                                 vk::ColorComponentFlagBits::eR |       // ColorWriteMask
                                                                 vk::ColorComponentFlagBits::eG |
                                                                 vk::ColorComponentFlagBits::eB |
                                                                 vk::ColorComponentFlagBits::eA);

    vkhlf::PipelineColorBlendStateCreateInfo noColorBlend(false,
                                                          vk::LogicOp::eNoOp,
                                                          noColorBlendAttachment,
                                                          { 1.0f, 1.0f, 1.0f, 1.0f });

    vkhlf::PipelineDynamicStateCreateInfo dynamic({ vk::DynamicState::eViewport,
                                                    vk::DynamicState::eScissor });

    mShader = device->createGraphicsPipeline(pipelineCache,
                                             {},
                                             { vertexStage, fragmentStage },
                                             vertexInput,
                                             assembly,
                                             nullptr,
                                             viewport,
                                             rasterization,
                                             multiSample,
                                             enableDepthStencil,
                                             noColorBlend,
                                             dynamic,
                                             modelData.mPipelineLayout,
                                             mParent->GetRenderPass());
  }

  void VkRenderToScreen::ScreenShader::Destroy()
  {
    mShader.reset();
  }

  void VkRenderToScreen::ScreenShader::Bind(std::shared_ptr<vkhlf::CommandBuffer> aCBO)
  {
    aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics, mShader);
  }



  std::string VkRenderToScreen::ScreenShader::GenerateFragmentShader()
  {
    std::stringstream ss;

    // begin
    ss << "#version 450\n"
       << "\n";

    // UBOs
    ss << "//========================================\n"
       << "// UBO Buffers\n"
       << "\n"
       << "\n"
       << "\n";

    
    // Samplers
    ss << "//========================================\n"
       << "// Samplers \n";

    auto& samplerData = mSibling->GetSamplerData();
    for (auto&[name, drawerType] : samplerData)
    {
      ss << fmt::format("layout(binding = UBO_{}_BINDING) uniform sampler2D {}Sampler;\n", name, name);
    }


    ss << "\n"
       << "\n"
       << "\n";

    // Fragment Shader Inputs | Vertex Shader Outputs
    ss << "//========================================\n"
       << "// Fragment Shader Inputs | Vertex Shader Outputs\n"
       << "layout(location = 0) in vec4 inColor;\n"
       << "layout(location = 1) in vec2 inTextureCoordinates;\n"
       << "\n"
       << "// ------------------------\n"
       << "// Output of Fragment\n"
       << "layout(location = 0) out vec4 outFragColor;\n"
       << "\n"
       << "\n";

    // Functions
    ss << "//========================================\n"
       << "// Functions\n"
       << "\n";

    // saturate
    ss << "// ------------------------\n"
       << "// Saturate:\n"
       << "// Clamps a given value to the interval [0-1]\n"
       << "vec4 saturate(vec4 aValue)\n"
       << "{\n"
       << "  return clamp(aValue, 0.0f, 1.0f);\n"
       << "}\n"
       << "vec3 saturate(vec3 aValue)\n"
       << "{\n"
       << "  return clamp(aValue, 0.0f, 1.0f);\n"
       << "}\n"
       << "vec2 saturate(vec2 aValue)\n"
       << "{\n"
       << "  return clamp(aValue, 0.0f, 1.0f);\n"
       << "}\n"
       << "float saturate(float aValue)\n"
       << "{\n"
       << "  return clamp(aValue, 0.0f, 1.0f);\n"
       << "}\n"
       << "\n"
       << "\n"
       << "\n";

    // main
    ss << "// ------------------------\n"
       << "// Main:\n"
       << "// Entry Point of Shader\n"
       << "void main()\n"
       << "{\n"
       << "  // fragment color\n"
       << "  vec4 col = vec4(0,0,0,0);\n";

    for (auto& [name, drawerType] : samplerData)
    {
      ss << fmt::format("  // {}\n", name);

      switch (drawerType)
      {
        case DrawerTypeCombination::AdditiveBlend:
        {
          ss << fmt::format("  col = saturate( col + texture({}Sampler, inTextureCoordinates.xy) );\n\n", name);
          break;
        }
        case DrawerTypeCombination::AlphaBlend:
        {
          ss << fmt::format("  vec4 {}color = texture({}Sampler, inTextureCoordinates.xy);\n", name, name);
          ss << fmt::format("  col = saturate( vec4(((1.0f - {}color.w) * col.xyz), (1.0f - {}color.w)) + \n"
                            "                  vec4(({}color.w * {}color.xyz), {}color.w) );\n\n",
                            name,
                            name, 
                            name, 
                            name, 
                            name);
          break;
        }
        case DrawerTypeCombination::MultiplicativeBlend:
        {
          ss << fmt::format("  col = saturate( col * texture({}Sampler, inTextureCoordinates.xy) );\n\n", name);
          break;
        }
        case DrawerTypeCombination::Opaque:
        {
          ss << fmt::format("  vec4 {}color = texture({}Sampler, inTextureCoordinates.xy);\n", name, name);
          ss << fmt::format("  col = saturate( vec4(((1.0f - {}color.w) * col.xyz), (1.0f - {}color.w)) + \n"
                            "                  {}color );\n\n",
                            name,
                            name,
                            name);
          break;
        }
        case DrawerTypeCombination::DoNotInclude:
        {
          ss << fmt::format("  // Not Included\n");
          ss << fmt::format("  vec4 {}color = texture({}Sampler, inTextureCoordinates.xy);\n\n", name, name);
          break;
        }
        case DrawerTypeCombination::DefaultCombination: // alpha blend
        default:
        {
          ss << fmt::format("  vec4 {}color = texture({}Sampler, inTextureCoordinates.xy);\n", name, name);
          ss << fmt::format("  col = saturate( vec4(((1.0f - {}color.w) * col.xyz), (1.0f - {}color.w)) + \n"
                            "                  vec4(({}color.w * {}color.xyz), {}color.w) );\n\n",
                            name,
                            name,
                            name,
                            name,
                            name);
          break;
        }
      }
    }

    ss << "  outFragColor = col;\n"
       << "}\n";

    // end
    ss << "\n";

    return ss.str();
  }
}
