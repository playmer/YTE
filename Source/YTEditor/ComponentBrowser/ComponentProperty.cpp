#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"

namespace YTEditor
{

  template <>
  void ComponentProperty<QStringList>::BaseSaveToEngine()
  {
    QStringList value = this->GetPropertyValues();
    mSetter->Invoke(mParentComponent->GetEngineComponent(), value[0].toStdString());
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
    YTE::Any currentString = mGetter->Invoke(mParentComponent->GetEngineComponent());

    //TODO: need to set the current string but how???
  }



}