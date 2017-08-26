#pragma once

#include "YTE/Core/PrivateImplementation.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

namespace YTE
{
  struct InstantiatedMesh
  {
    InstantiatedMesh()
      : mPosition(0.0f, 0.0f, 0.0f),
        mScale(1.0f, 1.0f, 1.0f),
        mRotation(),
        mMesh(nullptr)
    {

    }

    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::quat mRotation;

    Mesh *mMesh;

    PrivateImplementationDynamic mData;
  };
}
