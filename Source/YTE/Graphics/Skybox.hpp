#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class Skybox : public Component
  {
  public:
    YTEDeclareType(Skybox);
    Skybox(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Skybox();

    void Initialize() override;
    void CreateSkybox();

    std::string GetTexture()
    {
      return mTextureName;
    }

    void SetTexture(std::string &aTexture)
    {
      if (aTexture != mTextureName)
      {
        mTextureName = aTexture;

        if (false == mConstructing)
        {
          CreateSkybox();
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
    std::unique_ptr<InstantiatedModel> mInstantiatedSkybox;
    bool mConstructing = true;
  };
}