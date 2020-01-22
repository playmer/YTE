#include "YTEditor/YTELevelEditor/Widgets/ComponentBrowser/HeaderListProperty.hpp"

namespace YTEditor
{

  template <>
  void HeaderListProperty<QStringList>::BaseSaveToEngine()
  {
    QStringList value = this->GetPropertyValues();
    auto stdString = value[0].toStdString();

    YTE::Function *setter = mEngineProp->GetSetter();
    setter->Invoke(mObject, stdString);
  }


  template <>
  void HeaderListProperty<QStringList>::SaveToEngine()
  {
    // Add command to main window undo redo
    BaseSaveToEngine();
  }

  template <>
  void HeaderListProperty<QStringList>::ReloadValueFromEngine()
  {
    YTE::Function *getter = mEngineProp->GetGetter();
    YTE::Any currentString = getter->Invoke(mObject);

    //TODO: need to set the current string but how???
  }



}