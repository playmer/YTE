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
    RegisterType<InstantiatedModel>();
    TypeBuilder<InstantiatedModel> builder;
  }


  InstantiatedModel::InstantiatedModel()
    : mMesh(nullptr)
  {

  }
}
