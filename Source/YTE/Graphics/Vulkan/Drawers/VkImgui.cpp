// Based on the ImGui GLFW binding. The GLFW stuff needs to be rewritten in terms of YTE.

// ImGui GLFW binding with Vulkan + shaders
// FIXME: Changes of ImTextureID aren't supported by this binding! Please, someone add it!

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 5 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXX_CreateFontsTexture(), ImGui_ImplXXXX_NewFrame(), ImGui_ImplXXXX_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <imgui.h>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkImgui.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"

namespace YTE
{
  YTEDefineType(VkImguiDrawer)
  {
    RegisterType<VkImguiDrawer>();
    TypeBuilder<VkImguiDrawer> builder;
  }

  VkImguiDrawer::VkImguiDrawer(VkRenderedSurface *aSurface,
                               vk::Format aColorFormat,
                               vk::Format aDepthFormat,
                               std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                               ViewData* aView,
                               DrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     aView,
                     "VkImguiDrawer_" + aView->mName,
                     aCombinationType)
  {
    VkRenderTarget::Initialize();
    Initialize();
  }

  VkImguiDrawer::~VkImguiDrawer()
  {
  }

  void VkImguiDrawer::Initialize()
  {
    OPTICK_EVENT();

    mView = mParentViewData->mView;
    auto owner = mView->GetOwner();
    auto guidString = owner->GetGUID().ToString();;
    mContext = owner->GetComponent<ImguiLayer>();

    mModelName = fmt::format("Imgui_Model_{}", guidString);
    mTextureName = fmt::format("Imgui_Texture_{}", guidString);

    mContext->SetCurrentContext();

    ImGuiIO& io = ImGui::GetIO();
    
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    size_t uploadSize = width * height * 4 * sizeof(char);

    mSurface->GetRenderer()->CreateTexture(mTextureName,
                                           std::vector<u8>{pixels, pixels + uploadSize},
                                           TextureLayout::RGBA,
                                           width,
                                           height,
                                           0,
                                           1,
                                           vk::ImageViewType::e2D);

    owner->GetEngine()->RegisterEvent<&VkImguiDrawer::PreFrameUpdate>(Events::PreFrameUpdate, this);
  }

  static const std::string imguiStr{ "Imgui" };

  void VkImguiDrawer::PreFrameUpdate(LogicUpdate *aUpdate)
  {
    UnusedArguments(aUpdate);
    OPTICK_EVENT();

    mContext->SetCurrentContext();

    auto submesh = mContext->GetSubmeshData();

    ImGui::Render();
    auto drawData = ImGui::GetDrawData();

    submesh.mTextureData.emplace_back(mTextureName, TextureViewType::e2D, SubmeshData::TextureType::Diffuse);
    //submesh.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Specular);
    //submesh.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Normal);

    submesh.mShaderSetName = imguiStr;

    submesh.mVertexData.Clear();
    submesh.mIndexData.clear();

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
      OPTICK_EVENT("VkImgui CommandList Building");

      const ImDrawList* cmd_list = drawData->CmdLists[n];

      for (int i = 0; i < cmd_list->VtxBuffer.Size; ++i)
      {
        auto &theirVert = cmd_list->VtxBuffer.Data[i];
        Vertex vert;
        auto color = ImVec4(ImColor(theirVert.col));

        vert.mPosition = glm::vec3{ theirVert.pos.x, theirVert.pos.y, 0.0f };
        vert.mTextureCoordinates = glm::vec3{ theirVert.uv.x, theirVert.uv.y, 0.0f };
        vert.mColor = glm::vec4{ color.x, color.y, color.z, color.w };
        submesh.mVertexData.AddVertex(vert);
      }

      for (int i = 0; i < cmd_list->IdxBuffer.Size; ++i)
      {
        submesh.mIndexData.emplace_back(cmd_list->IdxBuffer.Data[i]);
      }
    }

    auto &instantiatedModel = mContext->GetInstantiatedModel();
    
    // TODO: Shouldn't have to reset here, but we're doing some weird stuff that's clashing with the
    //       VkRenderers need to clean up deleted things. So we need to mark the Mesh for deletion
    //       (by deleting the only instance that exists, ie: this one) so that we can safely remove it
    //       from the deletion list when we do the force update in CreateSimpleMesh.
    //
    //       The ImguiRenderer needs to fix this on their side such that we're updating an existing mesh
    //       with a variable number of verts, but right now we're waiting for the Renderer to have that
    //       capability. (Although maybe the particle system is already doing that? Unsure.
    //
    //       The renderer side needs to simply get smarter about how the Force update is in regards to 
    //       objects that get deleted out from under it. Maybe all objects need to be delay deleted and 
    //       we give them temporary new names on force delete, unsure.
    instantiatedModel.reset();

    if (submesh.mVertexData.mPositionData.empty())
    {
      return;
    }

    auto renderer = mSurface->GetRenderer();
    auto mesh = renderer->CreateSimpleMesh(mModelName, submesh, true);
    instantiatedModel = renderer->CreateModel(mView, mesh);

    UBOs::Model modelUBO;

    auto width = mView->GetWindow()->GetWidth();
    auto height = mView->GetWindow()->GetHeight();

    glm::vec3 scale{ 2.f / width, 2.f / height, 0.f };
    glm::vec3 translate{ -(width / 2.f), -(height / 2.f), 0.f };

    modelUBO.mModelMatrix = glm::translate(glm::scale(glm::mat4{}, scale), translate);
    mContext->GetInstantiatedModel()->UpdateUBOModel(modelUBO);

    auto vkmesh = static_cast<VkInstantiatedModel*>(instantiatedModel.get())->GetVkMesh();

    // We get the sub meshes that use the current shader, then draw them.
    auto range = vkmesh->mSubmeshMap.equal_range(imguiStr);

    mVkSubmesh = range.first->second;
    auto model = static_cast<VkInstantiatedModel*>(instantiatedModel.get());
    mPipelineData = &(model->mPipelineData.at(mVkSubmesh));

    for (auto &shader : mParentViewData->mShaders)
    {
      if (shader.first == imguiStr)
      {
        mShader = shader.second.get();
        break;
      }
    }
  }

  void VkImguiDrawer::RenderFull(std::unordered_map<std::string, std::unique_ptr<VkMesh>> &aMeshes)
  {
    UnusedArguments(aMeshes);

    OPTICK_EVENT();

    ++(*mCBOB);
    auto [commandBuffer, renderingFence] = **mCBOB;

    commandBuffer->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mRenderPass);
    Render(commandBuffer);
    commandBuffer->end();
  }

  void VkImguiDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    OPTICK_EVENT();

    mContext->SetCurrentContext();
    auto drawData = ImGui::GetDrawData();

    auto width = static_cast<float>(mData.mExtent.width);
    auto height = static_cast<float>(mData.mExtent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    aCBO->setViewport(0, viewport);

    vk::Rect2D scissor{ { 0, 0 }, mData.mExtent };
    aCBO->setScissor(0, scissor);
    aCBO->setLineWidth(1.0f);

    if (nullptr == mContext->GetInstantiatedModel())
    {
      return;
    }
    
    aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
                       mShader->mShaderNoCull);

    aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                             mPipelineData->mPipelineLayout,
                             0,
                             mPipelineData->mDescriptorSet,
                             nullptr);

    auto submesh = mVkSubmesh->mSubmesh;


    auto& vbd = submesh->mVertexBufferData;

    std::vector<u64> vertexBufferOffsets;
    std::vector<std::shared_ptr<vkhlf::Buffer>> vertexBuffersToBind;

    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mPositionBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mTextureCoordinatesBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mNormalBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mColorBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mTangentBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBinormalBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBitangentBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneWeightsBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneWeights2Buffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneIDsBuffer));
    vertexBuffersToBind.emplace_back(GetBuffer(vbd.mBoneIDs2Buffer));

    vertexBufferOffsets.resize(vertexBuffersToBind.size(), 0);

    aCBO->bindVertexBuffers(0, vertexBuffersToBind, vertexBufferOffsets);

    aCBO->bindIndexBuffer(GetBuffer(submesh->mIndexBuffer),
                          0,
                          vk::IndexType::eUint32);

    // Render the command lists:
    int vtx_offset = 0;
    int idx_offset = 0;

    for (int n = 0; n < drawData->CmdListsCount; ++n)
    {
      ImDrawList const *commandList = drawData->CmdLists[n];

      for (auto &commandBuffer : commandList->CmdBuffer)
      {
        if (commandBuffer.UserCallback)
        {
          commandBuffer.UserCallback(commandList, &commandBuffer);
        }
        else
        {
          vk::Rect2D scissorInner;
          scissorInner.offset.x = (int32_t)(commandBuffer.ClipRect.x) > 0 ? (int32_t)(commandBuffer.ClipRect.x) : 0;
          scissorInner.offset.y = (int32_t)(commandBuffer.ClipRect.y) > 0 ? (int32_t)(commandBuffer.ClipRect.y) : 0;
          scissorInner.extent.width = (uint32_t)(commandBuffer.ClipRect.z - commandBuffer.ClipRect.x);
          scissorInner.extent.height = (uint32_t)(commandBuffer.ClipRect.w - commandBuffer.ClipRect.y + 1); // FIXME: Why +1 here?
          
          aCBO->setScissor(0, scissorInner);
          aCBO->drawIndexed(static_cast<u32>(commandBuffer.ElemCount),
                            1,
                            idx_offset,
                            vtx_offset,
                            0);
        }

        idx_offset += commandBuffer.ElemCount;
      }

      vtx_offset += commandList->VtxBuffer.Size;
    }
  }
}