#include "YTE/Graphics/BaseModel.hpp"

namespace YTE
{
  YTEDefineEvent(UpdateVisibilityEvent);
  YTEDefineType(UpdateVisibilityEvent) { RegisterType<UpdateVisibilityEvent>(); }

  YTEDefineType(BaseModel)
  {
    RegisterType<BaseModel>();

    YTEBindProperty(&BaseModel::GetVisibility, &BaseModel::SetVisibility, "Visibility")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }
  
  BaseModel::BaseModel(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }
}