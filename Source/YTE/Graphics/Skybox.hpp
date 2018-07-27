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
    Skybox(Composition *aOwner, Space *aSpace);
    ~Skybox();

    void AssetInitialize() override;
    void NativeInitialize() override;
    void CreateSkybox();

    std::string GetTexture()
    {
      return mTextureName;
    }

    void SetTexture(std::string &aTexture);

    void CreateTransform();

    void TransformUpdate(TransformChanged *aEvent);

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::unique_ptr<InstantiatedModel> mInstantiatedSkybox;
    std::string mTextureName;
    u32 mSubdivisions;
    bool mConstructing = true;
  };
}