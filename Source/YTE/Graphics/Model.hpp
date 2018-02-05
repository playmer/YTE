//////////////////////////
// Author: Andrew Griffin
//////////////////////////

#pragma once

#ifndef YTE_Graphics_Model_hpp
#define YTE_Graphcis_Model_hpp

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

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

  class Model : public Component
  {
  public:
    YTEDeclareType(Model);
    Model(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~Model() override;
    void Initialize() override;

    void Reload();

    bool CanAnimate();
    

    /////////////////////////////////
    // Events
    /////////////////////////////////
    void TransformUpdate(TransformChanged *aEvent);

    /////////////////////////////////
    // Getter / Setter
    /////////////////////////////////
    void SetMesh(std::string &aName);

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
      YTEUnusedArgument(aBool);
      Destroy();
      Create();
    }

    void SetShading(std::string aName);

    Mesh* GetMesh();

    bool GetReload()
    {
      return false;
    }

    std::string GetMeshName()
    {
      return mMeshName;
    }

    InstantiatedModel* GetInstantiatedModel()
    {
      return mInstantiatedModel.get();
    }

    std::string GetShading()
    {
      return mShadingName;
    }

    void SetInstanced(bool mInstanced);
    bool GetInstanced();


    bool GetBackfaceCulling();
    void SetBackfaceCulling(bool aCulling);

  private:
    void Create();  // tells renderer to create mesh
    void Destroy(); // tells renderer to remove this instantiation
    void CreateTransform();

    std::string mMeshName;
    std::string mShadingName;
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;
    std::unique_ptr<InstantiatedModel> mInstantiatedModel;
    bool mConstructing;
    Animation *mAnimation;
    Engine *mEngine;

    bool mBackfaceCulling;
  };
}

#endif
