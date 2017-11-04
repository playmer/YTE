#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"

namespace YTEditor
{

  template <>
  void ComponentProperty<QStringList>::BaseSaveToEngine()
  {
    YTE::Component *comp = this->mParentComponent->GetEngineComponent();
    YTE::Type *type = comp->GetType();
    YTE::OrderedMultiMap<std::string, std::unique_ptr<YTE::Property>>& propMap = type->GetProperties();
    auto it_prop = propMap.FindFirst(this->GetName());

    QStringList value = this->GetPropertyValues();
    YTE::Function *setter = it_prop->second.get()->GetSetter();
    setter->Invoke(mParentComponent->GetEngineComponent(), value[0].toStdString());
  }


  template <>
  void ComponentProperty<QStringList>::SaveToEngine()
  {
    // Add command to main window undo redo
    BaseSaveToEngine();
  }

  template <>
  void ComponentProperty<QStringList>::ReloadValueFromEngine()
  {
    YTE::Component * comp = this->mParentComponent->GetEngineComponent();
    YTE::Type * type = comp->GetType();
    YTE::OrderedMultiMap<std::string, std::unique_ptr<YTE::Property>>& propMap = type->GetProperties();
    auto it_prop = propMap.FindFirst(this->GetName());

    YTE::Function *getter = it_prop->second.get()->GetGetter();
    YTE::Any currentString = getter->Invoke(mParentComponent->GetEngineComponent());

    //TODO: need to set the current string but how???
  }



}