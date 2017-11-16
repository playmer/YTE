//////////////////////////
// Author: Andrew Griffin
//////////////////////////

#pragma once

#ifndef YTE_Graphics_Model_hpp
#define YTE_Graphcis_Model_hpp

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

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


    /////////////////////////////////
    // Events
    /////////////////////////////////
    void PositionUpdate(TransformChanged *aEvent);
    void RotationUpdate(TransformChanged *aEvent);
    void ScaleUpdate(TransformChanged *aEvent);

    /////////////////////////////////
    // Gettor / Settor
    /////////////////////////////////
    void SetMesh(std::string aName);

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

    std::shared_ptr<Mesh> GetMesh();

    bool GetReload()
    {
      return false;
    }

    std::string GetMeshName()
    {
      return mMeshName;
    }

  private:
    void Create();  // tells renderer to create mesh
    void Destroy(); // tells renderer to remove this instantiation
    void CreateTransform();

    std::string mMeshName;
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;
    std::shared_ptr<InstantiatedModel> mInstantiatedModel;
    bool mConstructing;
  };
}


#endif
