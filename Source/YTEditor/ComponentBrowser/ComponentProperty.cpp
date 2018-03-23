#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"

namespace YTEditor
{

  template <>
  void ComponentProperty<QStringList>::BaseSaveToEngine()
  {
    // current list of strings
    QStringList strList = GetPropertyValues();

    // current str is always first
    std::string value = strList[0].toStdString();

    // set engine property value to new string
    mSetter->Invoke(mParentComponent->GetEngineComponent(), value);
  }


  template <>
  void ComponentProperty<QStringList>::SaveToEngine()
  {
    // engine component that owns property
    YTE::Component *cmp = mParentComponent->GetEngineComponent();

    // current value on engine side
    YTE::Any oldVal = mGetter->Invoke(mParentComponent->GetEngineComponent());

    MainWindow *mainWindow = mParentComponent->GetMainWindow();
    
    // notify that object instance changed (different from archetype now)
    ArchetypeTools *archTools = mainWindow->GetComponentBrowser().GetArchetypeTools();
    archTools->IncrementChanges();

    // current list of strings
    QStringList strList = GetPropertyValues();

    // current str is always first
    std::string value = strList[0].toStdString();

    YTE::Any modVal = YTE::Any(value);

    // check if the property ACTUALLY changed
    if (oldVal.As<std::string>() == value)
    {
      return;
    }

    std::string name = mEngineProperty->GetName();

    // property changed command to be inserted
    auto cmd = std::make_unique<ChangePropValCmd>(name,
                                                  cmp->GetGUID(),
                                                  oldVal,
                                                  modVal,
                                                  mainWindow);

    // put command on undo stack
    mainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));

    // save the value to the engine component
    BaseSaveToEngine();
  }

  template <>
  void ComponentProperty<QStringList>::ReloadValueFromEngine()
  {
    YTE::Any currentString = mGetter->Invoke(mParentComponent->GetEngineComponent());

    mSetter->Invoke(mParentComponent->GetEngineComponent(), currentString);
  }



}