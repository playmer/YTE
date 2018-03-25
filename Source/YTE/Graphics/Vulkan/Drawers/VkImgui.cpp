// Based on the ImGui GLFW binding. The GLFW stuff needs to be rewritten in terms of YTE.

// ImGui GLFW binding with Vulkan + shaders
// FIXME: Changes of ImTextureID aren't supported by this binding! Please, someone add it!

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 5 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXX_CreateFontsTexture(), ImGui_ImplXXXX_NewFrame(), ImGui_ImplXXXX_Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <imgui.h>

// GLFW


#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"
#include "YTE/Graphics/Vulkan/Drawers/VkImgui.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkInstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VkShader.hpp"

//bool ImGui_ImplGlfwVulkan_CreateFontsTexture(VkCommandBuffer command_buffer)
//{
//  ImGuiIO& io = ImGui::GetIO();
//
//  unsigned char* pixels;
//  int width, height;
//  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
//  size_t upload_size = width * height * 4 * sizeof(char);
//
//  VkResult err;
//
//  // Create the Image:
//  {
//    VkImageCreateInfo info = {};
//    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//    info.imageType = VK_IMAGE_TYPE_2D;
//    info.format = VK_FORMAT_R8G8B8A8_UNORM;
//    info.extent.width = width;
//    info.extent.height = height;
//    info.extent.depth = 1;
//    info.mipLevels = 1;
//    info.arrayLayers = 1;
//    info.samples = VK_SAMPLE_COUNT_1_BIT;
//    info.tiling = VK_IMAGE_TILING_OPTIMAL;
//    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    err = vkCreateImage(g_Device, &info, g_Allocator, &g_FontImage);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//    VkMemoryRequirements req;
//    vkGetImageMemoryRequirements(g_Device, g_FontImage, &req);
//    VkMemoryAllocateInfo alloc_info = {};
//    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    alloc_info.allocationSize = req.size;
//    alloc_info.memoryTypeIndex = ImGui_ImplGlfwVulkan_MemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
//    err = vkAllocateMemory(g_Device, &alloc_info, g_Allocator, &g_FontMemory);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//    err = vkBindImageMemory(g_Device, g_FontImage, g_FontMemory, 0);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//  }
//
//  // Create the Image View:
//  {
//    VkImageViewCreateInfo info = {};
//    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//    info.image = g_FontImage;
//    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
//    info.format = VK_FORMAT_R8G8B8A8_UNORM;
//    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    info.subresourceRange.levelCount = 1;
//    info.subresourceRange.layerCount = 1;
//    err = vkCreateImageView(g_Device, &info, g_Allocator, &g_FontView);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//  }
//
//  // Update the Descriptor Set:
//  {
//    VkDescriptorImageInfo desc_image[1] = {};
//    desc_image[0].sampler = g_FontSampler;
//    desc_image[0].imageView = g_FontView;
//    desc_image[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//    VkWriteDescriptorSet write_desc[1] = {};
//    write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//    write_desc[0].dstSet = g_DescriptorSet;
//    write_desc[0].descriptorCount = 1;
//    write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    write_desc[0].pImageInfo = desc_image;
//    vkUpdateDescriptorSets(g_Device, 1, write_desc, 0, NULL);
//  }
//
//  // Create the Upload Buffer:
//  {
//    VkBufferCreateInfo buffer_info = {};
//    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    buffer_info.size = upload_size;
//    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//    err = vkCreateBuffer(g_Device, &buffer_info, g_Allocator, &g_UploadBuffer);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//    VkMemoryRequirements req;
//    vkGetBufferMemoryRequirements(g_Device, g_UploadBuffer, &req);
//    g_BufferMemoryAlignment = (g_BufferMemoryAlignment > req.alignment) ? g_BufferMemoryAlignment : req.alignment;
//    VkMemoryAllocateInfo alloc_info = {};
//    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    alloc_info.allocationSize = req.size;
//    alloc_info.memoryTypeIndex = ImGui_ImplGlfwVulkan_MemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
//    err = vkAllocateMemory(g_Device, &alloc_info, g_Allocator, &g_UploadBufferMemory);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//    err = vkBindBufferMemory(g_Device, g_UploadBuffer, g_UploadBufferMemory, 0);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//  }
//
//  // Upload to Buffer:
//  {
//    char* map = NULL;
//    err = vkMapMemory(g_Device, g_UploadBufferMemory, 0, upload_size, 0, (void**)(&map));
//    ImGui_ImplGlfwVulkan_VkResult(err);
//    memcpy(map, pixels, upload_size);
//    VkMappedMemoryRange range[1] = {};
//    range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//    range[0].memory = g_UploadBufferMemory;
//    range[0].size = upload_size;
//    err = vkFlushMappedMemoryRanges(g_Device, 1, range);
//    ImGui_ImplGlfwVulkan_VkResult(err);
//    vkUnmapMemory(g_Device, g_UploadBufferMemory);
//  }
//  // Copy to Image:
//  {
//    VkImageMemoryBarrier copy_barrier[1] = {};
//    copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//    copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//    copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//    copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    copy_barrier[0].image = g_FontImage;
//    copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    copy_barrier[0].subresourceRange.levelCount = 1;
//    copy_barrier[0].subresourceRange.layerCount = 1;
//    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);
//
//    VkBufferImageCopy region = {};
//    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    region.imageSubresource.layerCount = 1;
//    region.imageExtent.width = width;
//    region.imageExtent.height = height;
//    region.imageExtent.depth = 1;
//    vkCmdCopyBufferToImage(command_buffer, g_UploadBuffer, g_FontImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//    VkImageMemoryBarrier use_barrier[1] = {};
//    use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//    use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//    use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//    use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//    use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//    use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    use_barrier[0].image = g_FontImage;
//    use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    use_barrier[0].subresourceRange.levelCount = 1;
//    use_barrier[0].subresourceRange.layerCount = 1;
//    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
//  }
//
//  // Store our identifier
//  io.Fonts->TexID = (void *)(intptr_t)g_FontImage;
//
//  return true;
//}
//
//
//






















namespace YTE
{
















  YTEDefineType(VkImguiDrawer)
  {
    YTERegisterType(VkImguiDrawer);
  }

  VkImguiDrawer::VkImguiDrawer(VkRenderedSurface *aSurface,
                               vk::Format aColorFormat,
                               vk::Format aDepthFormat,
                               std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                               ViewData* aView,
                               std::string aName,
                               YTEDrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     aView,
                     "VkImguiDrawer_" + aName,
                     aCombinationType)
  {

  }

  VkImguiDrawer::VkImguiDrawer(VkRenderedSurface *aSurface,
                                               vk::Format aColorFormat,
                                               vk::Format aDepthFormat,
                                               std::shared_ptr<vkhlf::Surface>& aVulkanSurface,
                                               std::string aName,
                                               YTEDrawerTypeCombination aCombinationType)
    : VkRenderTarget(aSurface,
                     aColorFormat,
                     aDepthFormat,
                     aVulkanSurface,
                     "VkImguiDrawer_" + aName,
                     aCombinationType)
  {

  }

  VkImguiDrawer::~VkImguiDrawer()
  {
  }

  static const std::string imguiStr{ "Imgui" };

  void VkImguiDrawer::GetRenderData()
  {
    ImGui::SetCurrentContext(mContext);
    auto drawData = ImGui::GetDrawData();

    VkResult err;
    ImGuiIO& io = ImGui::GetIO();

    std::string meshName = imguiStr;

    //mSubmesh.mDiffuseMap = mTextureName;
    mSubmesh.mDiffuseType = TextureViewType::e2D;
    mSubmesh.mShaderSetName = imguiStr;

    mSubmesh.mVertexBuffer.clear();
    mSubmesh.mIndexBuffer.clear();

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
      const ImDrawList* cmd_list = drawData->CmdLists[n];

      for (int i = 0; i < cmd_list->VtxBuffer.Size; ++i)
      {
        auto &theirVert = cmd_list->VtxBuffer.Data[i];
        Vertex vert;
        auto color = ImVec4(ImColor(theirVert.col));

        vert.mPosition = glm::vec3{ theirVert.pos.x, theirVert.pos.y, 0.0f };
        vert.mTextureCoordinates = glm::vec3{ theirVert.uv.x, theirVert.uv.y, 0.0f };
        vert.mColor = glm::vec4{ color.x, color.y, color.z, color.w };
      }

      for (int i = 0; i < cmd_list->IdxBuffer.Size; ++i)
      {
        mSubmesh.mIndexBuffer.emplace_back(cmd_list->IdxBuffer.Data[i]);
      }
    }

    std::vector<Submesh> submeshes{ mSubmesh };

    auto renderer = mSurface->GetRenderer();
    auto mesh = renderer->CreateSimpleMesh(meshName, submeshes, true);
    mInstantiatedModel = renderer->CreateModel(mView, mesh);

    UBOModel modelUBO;
    mInstantiatedModel->UpdateUBOModel(modelUBO);


    auto vkmesh = static_cast<VkMesh*>(mInstantiatedModel->GetMesh());

    // We get the sub meshes that use the current shader, then draw them.
    auto range = vkmesh->mSubmeshes.equal_range(imguiStr);

    mVkSubmesh = range.first->second.get();
    auto model = static_cast<VkInstantiatedModel*>(mInstantiatedModel.get());
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

  void VkImguiDrawer::RenderFull(const vk::Extent2D& aExtent,
                                         std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    mCBOB->NextCommandBuffer();
    auto cbo = mCBOB->GetCurrentCBO();
    cbo->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, mRenderPass);
    RenderBegin(cbo);
    Render(cbo, aExtent, aMeshes);
    RenderEnd(cbo);
    cbo->end();
  }

  void VkImguiDrawer::RenderBegin(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    YTEUnusedArgument(aCBO);
  }

  void VkImguiDrawer::Render(std::shared_ptr<vkhlf::CommandBuffer>& aCBO,
                                     const vk::Extent2D& extent,
                                     std::unordered_map<std::string, std::unique_ptr<VkMesh>>& aMeshes)
  {
    ImGui::SetCurrentContext(mContext);
    auto drawData = ImGui::GetDrawData();

    auto width = static_cast<float>(extent.width);
    auto height = static_cast<float>(extent.height);

    vk::Viewport viewport{ 0.0f, 0.0f, width, height, 0.0f,1.0f };
    aCBO->setViewport(0, viewport);

    vk::Rect2D scissor{ { 0, 0 }, extent };
    aCBO->setScissor(0, scissor);
    aCBO->setLineWidth(1.0f);
    
    auto model = static_cast<VkInstantiatedModel*>(mInstantiatedModel.get());


    aCBO->bindPipeline(vk::PipelineBindPoint::eGraphics,
                       mShader->mTriangles);

    aCBO->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                             mPipelineData->mPipelineLayout,
                             0,
                             mPipelineData->mDescriptorSet,
                             nullptr);

    aCBO->bindVertexBuffer(0,
                           mVkSubmesh->mVertexBuffer,
                           0);

    aCBO->bindIndexBuffer(mVkSubmesh->mIndexBuffer,
                          0,
                          vk::IndexType::eUint32);

    glm::vec2 scale{ 2.0f / width, 2.0f / height };

    aCBO->pushConstants<glm::vec2>(mPipelineData->mPipelineLayout,
                                   vk::ShaderStageFlagBits::eVertex,
                                   0,
                                   scale);


    glm::vec2 translate{ -1.0f, 1.0f };
    aCBO->pushConstants<glm::vec2>(mPipelineData->mPipelineLayout,
                                   vk::ShaderStageFlagBits::eVertex,
                                   0,
                                   translate);
    
    // Render the command lists:
    int vtx_offset = 0;
    int idx_offset = 0;
    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
      const ImDrawList* cmd_list = drawData->CmdLists[n];
      for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
      {
        const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
        if (pcmd->UserCallback)
        {
          pcmd->UserCallback(cmd_list, pcmd);
        }
        else
        {
          vk::Rect2D scissorInner;
          scissorInner.offset.x = (int32_t)(pcmd->ClipRect.x) > 0 ? (int32_t)(pcmd->ClipRect.x) : 0;
          scissorInner.offset.y = (int32_t)(pcmd->ClipRect.y) > 0 ? (int32_t)(pcmd->ClipRect.y) : 0;
          scissorInner.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
          scissorInner.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?

          aCBO->setScissor(0, scissorInner);
          aCBO->drawIndexed(static_cast<u32>(pcmd->ElemCount),
                            1,
                            idx_offset,
                            vtx_offset,
                            0);
        }
        idx_offset += pcmd->ElemCount;
      }
      vtx_offset += cmd_list->VtxBuffer.Size;
    }
  }

  void VkImguiDrawer::RenderEnd(std::shared_ptr<vkhlf::CommandBuffer>& aCBO)
  {
    YTEUnusedArgument(aCBO);
  }
}