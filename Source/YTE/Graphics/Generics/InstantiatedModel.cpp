///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"


namespace YTE
{
  YTEDefineType(InstantiatedModel)
  {
    YTERegisterType(InstantiatedModel);
  }


  InstantiatedModel::InstantiatedModel()
    : mMesh(nullptr)
  {

  }

  void InstantiatedModel::SetBackfaceCulling(bool aCulling)
  {
    mMesh->SetBackfaceCulling(aCulling);
    mMesh->RecreateShader();
  }

}
