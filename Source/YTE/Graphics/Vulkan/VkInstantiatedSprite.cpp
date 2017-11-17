#include "YTE/Graphics/Vulkan/VkInstantiatedSprite.hpp"

namespace YTE
{
  VkInstantiatedSprite::VkInstantiatedSprite(std::string &aTextureFile, VkRenderedSurface *aSurface)
  {

  }

  VkInstantiatedSprite::~VkInstantiatedSprite()
  {

  }

  void VkInstantiatedSprite::UpdateTransformation(glm::mat4 &aTransformation)
  {
    mTransformation = aTransformation;
  }

  void VkInstantiatedSprite::GraphicsDataUpdate(GraphicsDataUpdateVk *aEvent)
  {

  }
}