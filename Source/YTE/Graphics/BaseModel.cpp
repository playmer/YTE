#include "YTE/Graphics/BaseModel.hpp"

namespace YTE
{
  YTEDefineType(BaseModel)
  {
    YTERegisterType(BaseModel);
  }
  
  BaseModel::BaseModel(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }
}