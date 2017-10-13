#include <set>

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/StandardLibrary/Delegate.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vulkan/VkFunctionLoader.hpp"
#include "vulkan/vulkan.hpp"

namespace YTE
{
  struct MeshToRender
  {
    MeshToRender(std::shared_ptr<vkhlf::PipelineLayout> const *aPipelineLayout,
      std::shared_ptr<vkhlf::DescriptorSet> const *aDescriptorSet,
      std::shared_ptr<vkhlf::Buffer> const *aVertexBuffer,
      std::shared_ptr<vkhlf::Buffer> const *aIndexBuffer,
      u64 aIndexCount);

    MeshToRender(const MeshToRender &aRight);
    MeshToRender& operator=(const MeshToRender &aRight);

    std::shared_ptr<vkhlf::PipelineLayout> const *mPipelineLayout;
    std::shared_ptr<vkhlf::DescriptorSet> const *mDescriptorSet;
    std::shared_ptr<vkhlf::Buffer> const *mVertexBuffer;
    std::shared_ptr<vkhlf::Buffer> const *mIndexBuffer;
    u64 mIndexCount;
  };


  struct PipelineData
  {
    PipelineData(std::shared_ptr<vkhlf::Pipeline> aPipeline);

    std::shared_ptr<vkhlf::Pipeline> mPipeline;

    OrderedMap<u64, MeshToRender> mModels;
  };

}