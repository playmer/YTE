#include "YTE/Graphics/BaseModel.hpp"

namespace YTE
{
  YTEDefineEvent(UpdateVisibilityEvent);
  YTEDefineType(UpdateVisibilityEvent) { RegisterType<UpdateVisibilityEvent>();
    TypeBuilder<UpdateVisibilityEvent> builder; }

  YTEDefineType(BaseModel)
  {
    RegisterType<BaseModel>();
    TypeBuilder<BaseModel> builder;

    builder.Property<&BaseModel::GetVisibility, &BaseModel::SetVisibility>( "Visibility")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }
  
  BaseModel::BaseModel(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    UnusedArguments(aProperties);
  }
}