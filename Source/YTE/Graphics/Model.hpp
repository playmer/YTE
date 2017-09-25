#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Graphics/Renderer.hpp"

namespace YTE
{
  class Model : public Component
  {
  public:
    YTEDeclareType(Model);

    Model(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    void Update(LogicUpdate *aEvent);

    ~Model() override;

    InstantiatedMesh* GetInstantiatedMesh()
    {
      return mInstantiatedMesh.get();
    }

    void SetReload(bool)
    {
      if (false == mConstructing)
      {
        Destroy();
        Create();
      }
    }

    bool GetReload()
    {
      return false;
    }

    void SetMesh(std::string &aMesh)
    {
      if (mMesh == aMesh)
      {
        return;
      }

      mMesh = aMesh;

      if (false == mConstructing)
      {
        Destroy();
        Create();
      }
    }

    std::string& GetMesh()
    {
      return mMesh;
    }

  private:
    // Create and destroy the internal mesh.
    void Create();
    void Destroy();

    void SetUBO();
    void OnPositionChange(const TransformChanged *aEvent);
    void OnScaleChange(const TransformChanged *aEvent);
    void OnRotationChange(const TransformChanged *aEvent);

    std::string mMesh;

    bool mConstructing;

    Renderer *mRenderer;
    std::unique_ptr<InstantiatedMesh> mInstantiatedMesh;
    bool mUpdating;
  };
}