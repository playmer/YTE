//////////////////////////
// Author: Andrew Griffin
//////////////////////////

#pragma once

#ifndef YTE_Graphics_Model_hpp
#define YTE_Graphcis_Model_hpp

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/BaseModel.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class Window;

  YTEDeclareEvent(ModelChanged);

  class ModelChanged : public Event
  {
  public:
    YTEDeclareType(ModelChanged);

    Composition *Object;

  };

  class Model : public BaseModel
  {
  public:
    YTEDeclareType(Model);

    YTE_Shared Model(Composition* aOwner, Space* aSpace);
    YTE_Shared ~Model() override;

    YTE_Shared void AssetInitialize() override;
    YTE_Shared void NativeInitialize() override;

    YTE_Shared void Reload();

    YTE_Shared bool CanAnimate();
    

    /////////////////////////////////
    // Events
    /////////////////////////////////
    YTE_Shared void TransformUpdate(TransformChanged *aEvent);

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    YTE_Shared void SetMesh(std::string &aName);

    void SetMeshName(std::string aName)
    {
      SetMesh(aName);
    }

    void SetReload(bool aBool)
    {
      if (mConstructing)
      {
        return;
      }
      UnusedArguments(aBool);
      Destroy();
      Create();
    }

    YTE_Shared Mesh* GetMesh();

    bool GetReload()
    {
      return false;
    }

    std::string const& GetMeshName()
    {
      return mMeshName;
    }

    std::vector<InstantiatedModel*> GetInstantiatedModel() override
    {
      std::vector<InstantiatedModel*> toReturn;

      if (mInstantiatedModel)
      {
          toReturn.emplace_back(mInstantiatedModel.get());
      }
      
      return toReturn;
    }

    YTE_Shared void SetShading(std::string const &aName);
    std::string const& GetShading()
    {
      return mShadingName;
    }

  private:
    void Create();  // tells renderer to create mesh
    void Destroy(); // tells renderer to remove this instantiation
    void CreateTransform();

    std::string mMeshName;
    std::string mShadingName;
    std::unique_ptr<InstantiatedModel> mInstantiatedModel;
    Renderer* mRenderer;
    Window* mWindow;
    Transform* mTransform;
    UBOs::Model mUBOModel;
    Animation* mAnimation;
    Engine* mEngine;

    bool mConstructing;
    bool mBackfaceCulling;
  };
}

#endif
