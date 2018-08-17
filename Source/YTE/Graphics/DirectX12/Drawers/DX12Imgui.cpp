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
#include "YTE/Graphics/DirectX12/Drawers/DX12Imgui.hpp"
#include "YTE/Graphics/DirectX12/DX12Renderer.hpp"
#include "YTE/Graphics/DirectX12/Dx12RenderedSurface.hpp"
#include "YTE/Graphics/DirectX12/DX12InstantiatedModel.hpp"
#include "YTE/Graphics/DirectX12/DX12Shader.hpp"

namespace YTE
{
  YTEDefineType(Dx12ImguiDrawer)
  {
    RegisterType<Dx12ImguiDrawer>();
    TypeBuilder<Dx12ImguiDrawer> builder;
  }

  Dx12ImguiDrawer::Dx12ImguiDrawer(Dx12RenderedSurface *aSurface,
                                   //vk::Format aColorFormat,
                                   //vk::Format aDepthFormat,
                                   //std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                   DX12ViewData* aView,
                                   DrawerTypeCombination aCombinationType)
    : DX12RenderTarget(aSurface,
                       //aColorFormat,
                       //aDepthFormat,
                       //aVulkanSurface,
                       aView,
                       "Dx12ImguiDrawer_" + aView->mName,
                       aCombinationType)
  {
    DX12RenderTarget::Initialize();
    Initialize();
  }

  Dx12ImguiDrawer::~Dx12ImguiDrawer()
  {
  }

  void Dx12ImguiDrawer::Initialize()
  {
    //mView = mParentViewData->mView;
    //auto owner = mView->GetOwner();
    //auto guidString = owner->GetGUID().ToString();;
    //mContext = owner->GetComponent<ImguiLayer>();
    //
    //mModelName = fmt::format("Imgui_Model_{}", guidString);
    //mTextureName = fmt::format("Imgui_Texture_{}", guidString);
    //
    //mContext->SetCurrentContext();
    //
    //ImGuiIO& io = ImGui::GetIO();
    //
    //unsigned char* pixels;
    //int width, height;
    //io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    //size_t uploadSize = width * height * 4 * sizeof(char);
    //
    //mSurface->GetRenderer()->CreateTexture(mTextureName,
    //                                       std::vector<u8>{pixels, pixels + uploadSize},
    //                                       TextureLayout::RGBA,
    //                                       width,
    //                                       height,
    //                                       0,
    //                                       1,
    //                                       vk::ImageViewType::e2D);
    //
    //owner->GetEngine()->RegisterEvent<&Dx12ImguiDrawer::PreFrameUpdate>(Events::PreFrameUpdate, this);
  }

  static const std::string imguiStr{ "Imgui" };

  void Dx12ImguiDrawer::PreFrameUpdate(LogicUpdate *aUpdate)
  {
    //UnusedArguments(aUpdate);
    //YTEProfileFunction();
    //
    //mContext->SetCurrentContext();
    //
    //auto &submesh = mContext->GetSubmesh();
    //
    //ImGui::Render();
    //auto drawData = ImGui::GetDrawData();
    //
    //submesh.mDiffuseMap = mTextureName;
    //submesh.mDiffuseType = TextureViewType::e2D;
    //submesh.mShaderSetName = imguiStr;
    //
    //submesh.mVertexBuffer.clear();
    //submesh.mIndexBuffer.clear();
    //
    //for (int n = 0; n < drawData->CmdListsCount; n++)
    //{
    //  YTEProfileBlock("VkImgui CommandList Building");
    //
    //  const ImDrawList* cmd_list = drawData->CmdLists[n];
    //
    //  for (int i = 0; i < cmd_list->VtxBuffer.Size; ++i)
    //  {
    //    auto &theirVert = cmd_list->VtxBuffer.Data[i];
    //    Vertex vert;
    //    auto color = ImVec4(ImColor(theirVert.col));
    //
    //    vert.mPosition = glm::vec3{ theirVert.pos.x, theirVert.pos.y, 0.0f };
    //    vert.mTextureCoordinates = glm::vec3{ theirVert.uv.x, theirVert.uv.y, 0.0f };
    //    vert.mColor = glm::vec4{ color.x, color.y, color.z, color.w };
    //    submesh.mVertexBuffer.emplace_back(vert);
    //  }
    //
    //  for (int i = 0; i < cmd_list->IdxBuffer.Size; ++i)
    //  {
    //    submesh.mIndexBuffer.emplace_back(cmd_list->IdxBuffer.Data[i]);
    //  }
    //}
    //
    //auto &instantiatedModel = mContext->GetInstantiatedModel();
    //
    //if (submesh.mVertexBuffer.empty())
    //{
    //  instantiatedModel.reset();
    //  return;
    //}
    //
    //std::vector<Submesh> submeshes{ submesh };
    //
    //auto renderer = mSurface->GetRenderer();
    //auto mesh = renderer->CreateSimpleMesh(mModelName, submeshes, true);
    //instantiatedModel = renderer->CreateModel(mView, mesh);
    //
    //UBOModel modelUBO;
    //
    //auto width = mView->GetWindow()->GetWidth();
    //auto height = mView->GetWindow()->GetHeight();
    //
    //glm::vec3 scale{ 2.f / width, 2.f / height, 0.f };
    //glm::vec3 translate{ -(width / 2.f), -(height / 2.f), 0.f };
    //
    //modelUBO.mModelMatrix = glm::translate(glm::scale(glm::mat4{}, scale), translate);
    //mContext->GetInstantiatedModel()->UpdateUBOModel(modelUBO);
    //
    //auto vkmesh = static_cast<DX12InstantiatedModel*>(instantiatedModel.get())->GetVkMesh();
    //
    //// We get the sub meshes that use the current shader, then draw them.
    //auto range = vkmesh->mSubmeshes.equal_range(imguiStr);
    //
    //mDx12Submesh = range.first->second.get();
    //auto model = static_cast<DX12InstantiatedModel*>(instantiatedModel.get());
    //mPipelineData = &(model->mPipelineData.at(mDx12Submesh));
    //
    //for (auto &shader : mParentViewData->mShaders)
    //{
    //  if (shader.first == imguiStr)
    //  {
    //    mShader = shader.second.get();
    //    break;
    //  }
    //}
  }

  void Dx12ImguiDrawer::RenderFull(std::unordered_map<std::string, std::unique_ptr<DX12Mesh>> &aMeshes)
  {
    //UnusedArguments(aMeshes);
    //
    //mCBOB->NextCommandBuffer();
    //auto cbo = mCBOB->GetCurrentCBO();
    //cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mRenderPass);
    //RenderBegin(cbo);
    //Render(cbo);
    //RenderEnd(cbo);
    //cbo->end();
  }

  void Dx12ImguiDrawer::RenderBegin(/*std::shared_ptr<vkhlf::CommandBuffer>& aCBO*/)
  {
    //UnusedArguments(aCBO);
  }

  void Dx12ImguiDrawer::Render(/*std::shared_ptr<vkhlf::CommandBuffer>& aCBO*/)
  {
    //mContext->SetCurrentContext();
    //auto drawData = ImGui::GetDrawData();
    //
    //auto width = static_cast<float>(mData.mExtent.width);
    //auto height = static_cast<float>(mData.mExtent.height);
    //
    //vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    //aCBO->setViewport(0, viewport);
    //
    //vk::Rect2D scissor{ { 0, 0 }, mData.mExtent };
    //aCBO->setScissor(0, scissor);
    //aCBO->setLineWidth(1.0f);
    //
    //if (nullptr == mContext->GetInstantiatedModel())
    //{
    //  return;
    //}
    //
    //aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
    //                   mShader->mShaderNoCull);
    //
    //aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
    //                         mPipelineData->mPipelineLayout,
    //                         0,
    //                         mPipelineData->mDescriptorSet,
    //                         nullptr);
    //
    //aCBO->bindVertexBuffer(0,
    //                       mDx12Submesh->mVertexBuffer,
    //                       0);
    //
    //aCBO->bindIndexBuffer(mDx12Submesh->mIndexBuffer,
    //                      0,
    //                      vk::IndexType::eUint32);
    //
    //// Render the command lists:
    //int vtx_offset = 0;
    //int idx_offset = 0;
    //
    //for (int n = 0; n < drawData->CmdListsCount; ++n)
    //{
    //  ImDrawList const *commandList = drawData->CmdLists[n];
    //
    //  for (auto &commandBuffer : commandList->CmdBuffer)
    //  {
    //    if (commandBuffer.UserCallback)
    //    {
    //      commandBuffer.UserCallback(commandList, &commandBuffer);
    //    }
    //    else
    //    {
    //      vk::Rect2D scissorInner;
    //      scissorInner.offset.x = (int32_t)(commandBuffer.ClipRect.x) > 0 ? (int32_t)(commandBuffer.ClipRect.x) : 0;
    //      scissorInner.offset.y = (int32_t)(commandBuffer.ClipRect.y) > 0 ? (int32_t)(commandBuffer.ClipRect.y) : 0;
    //      scissorInner.extent.width = (uint32_t)(commandBuffer.ClipRect.z - commandBuffer.ClipRect.x);
    //      scissorInner.extent.height = (uint32_t)(commandBuffer.ClipRect.w - commandBuffer.ClipRect.y + 1); // FIXME: Why +1 here?
    //      
    //      aCBO->setScissor(0, scissorInner);
    //      aCBO->drawIndexed(static_cast<u32>(commandBuffer.ElemCount),
    //                        1,
    //                        idx_offset,
    //                        vtx_offset,
    //                        0);
    //    }
    //
    //    idx_offset += commandBuffer.ElemCount;
    //  }
    //
    //  vtx_offset += commandList->VtxBuffer.Size;
    //}
  }

  void Dx12ImguiDrawer::RenderEnd(/*std::shared_ptr<vkhlf::CommandBuffer>& aCBO*/)
  {
    //UnusedArguments(aCBO);
  }
}