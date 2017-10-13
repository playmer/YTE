///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

namespace YTE
{
  YTEDefineType(InstantiatedModel)
  {
    YTERegisterType(InstantiatedModel);
  }



  InstantiatedModel::InstantiatedModel()
  {

  }



  InstantiatedModel::~InstantiatedModel()
  {

  }



  void InstantiatedModel::UpdateUBOPerModel(UBOPerModel &aUBO)
  {
    mUBOPerModelData = aUBO;
  }
}
