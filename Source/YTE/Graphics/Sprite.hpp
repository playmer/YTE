#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class Sprite : public Component
  {
  public:
    YTEDeclareType(Sprite);
    Sprite(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Sprite();

    void Initialize() override;
    void CreateSprite();

    std::string GetTexture()
    {
      return mTextureName;
    }

    void SetTexture(std::string &aTexture)
    {
      if (aTexture != mTextureName &&
          0 != aTexture.size())
      {
        mTextureName = aTexture;

        if (mConstructing == false)
        {
          CreateSprite();
        }
      }
    }

    void CreateTransform();

    void TransformUpdate(TransformChanged *aEvent);

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::string mTextureName;
    std::unique_ptr<InstantiatedModel> mInstantiatedSprite;
    bool mConstructing;
  };
}
