#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

namespace YTE
{
  class Sprite : public Component
  {
  public:
    YTEDeclareType(Sprite);
    Sprite(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Sprite();

  private:
    Texture *mTexture;
    std::string mTextureName;
    std::unique_ptr<InstantiatedSprite> mInstantiatedModel;
  };
}