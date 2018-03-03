#include "YTEditor/ComponentBrowser/ComponentProperty.hpp"

namespace YTEditor
{

  template <>
  void ComponentProperty<QStringList>::BaseSaveToEngine()
  {
    YTE::Component *cmp = mParentComponent->GetEngineComponent();

    YTE::Any oldVal = mGetter->Invoke(mParentComponent->GetEngineComponent());

    MainWindow *mainWindow = mParentComponent->GetMainWindow();

    ArchetypeTools *archTools = mainWindow->GetComponentBrowser().GetArchetypeTools();

    QStringList val = this->GetPropertyValues();
    YTE::Any modVal = YTE::Any(val[0].toStdString());

    if (oldVal.As<std::string>() == val[0].toStdString())
    {
      return;
    }

    // Add command to main window undo redo
    std::string name = mEngineProperty->GetName();

    auto cmd = std::make_unique<ChangePropValCmd>(name,
      cmp->GetGUID(),
      oldVal,
      modVal,
      mainWindow);

    mainWindow->GetUndoRedo()->InsertCommand(std::move(cmd));

    archTools->IncrementChanges();

    // Add command to main window undo redo
    BaseSaveToEngine();
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
    mSetter->Invoke(mParentComponent->GetEngineComponent(), currentString);
  }



}