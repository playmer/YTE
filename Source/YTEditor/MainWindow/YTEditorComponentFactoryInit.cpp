#include "YTEditorComponentFactoryInit.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"

namespace YTEditor
{

  ComponentFactoryHelper::ComponentFactoryHelper(YTE::Engine *aEngine, YTE::FactoryMap *aComponentFactories)
  : mEngine(aEngine), 
    mComponentFactories(aComponentFactories)
  {

  }

  template <typename tComponent>
  void ComponentFactoryHelper::CreateComponentFactory()
  {
    YTE::Type *type = tComponent::GetStaticType();

    auto it = mComponentFactories->Find(type);

    if (it == mComponentFactories->end())
    {
      mComponentFactories->Emplace(type, std::make_unique<YTE::ComponentFactory<tComponent>>(mEngine));
    }
  }


  void ComponentFactoryInitialization(YTE::Engine * aEngine, YTE::FactoryMap &currComponentFactories)
  {
    ComponentFactoryHelper helper(aEngine, &currComponentFactories);

    helper.CreateComponentFactory<YTE::Translate>();
    helper.CreateComponentFactory<YTE::Scale>();
    helper.CreateComponentFactory<YTE::Rotate>();
  }



}