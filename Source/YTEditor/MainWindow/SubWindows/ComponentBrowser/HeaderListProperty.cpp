#include "YTEditor/MainWindow/SubWindows/ComponentBrowser/HeaderListProperty.hpp"

namespace YTEditor
{

  template <>
  void HeaderListProperty<QStringList>::BaseSaveToEngine()
  {
    QStringList value = this->GetPropertyValues();
    YTE::Function *setter = mEngineProp->GetSetter();
    setter->Invoke(mObject, value[0].toStdString());
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