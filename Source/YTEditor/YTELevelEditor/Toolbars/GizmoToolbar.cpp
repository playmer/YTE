#include <filesystem>

#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/Framework/ToolBarButton.hpp"

#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"

#include "YTEditor/YTELevelEditor/Widgets/ObjectBrowser/ObjectBrowser.hpp"

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

  GizmoToolbar::GizmoToolbar(Framework::MainWindow* aMainWindow)
    : Framework::ToolBar(aMainWindow)
  {
    std::experimental::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
    std::experimental::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/")};

    QString iconPath = (assetsDir.generic_string() + "select.png").c_str();
    mSelect = new Framework::ToolBarButton(this, iconPath);
    mSelect->connect(mSelect, &QPushButton::toggled, this, &GizmoToolbar::SelectToggled);

    iconPath = (assetsDir.generic_string() + "translate.png").c_str();
    mTranslate = new Framework::ToolBarButton(this, iconPath);
    mTranslate->connect(mTranslate, &QPushButton::toggled, this, &GizmoToolbar::TranslateToggled);

    iconPath = (assetsDir.generic_string() + "rotate.png").c_str();
    mRotate = new Framework::ToolBarButton(this, iconPath);
    mRotate->connect(mRotate, &QPushButton::toggled, this, &GizmoToolbar::RotateToggled);

    iconPath = (assetsDir.generic_string() + "scale.png").c_str();
    mScale = new Framework::ToolBarButton(this, iconPath);
    mScale->connect(mScale, &QPushButton::toggled, this, &GizmoToolbar::ScaleToggled);

    iconPath = (assetsDir.generic_string() + "switchAxes.png").c_str();
    mSwitchAxesMode = new Framework::ToolBarButton(this, iconPath);
    mSwitchAxesMode->SetIsUncheckable(true);
    mSwitchAxesMode->SetIsResettable(false);
    mSwitchAxesMode->connect(mSwitchAxesMode, &QPushButton::toggled, this, &GizmoToolbar::SwitchAxesModeToggled);
    mSwitchAxesMode->setToolTip("Switches from world to local axes for gizmos.");

    AddButton(mSelect);
    AddButton(mTranslate);
    AddButton(mRotate);
    AddButton(mScale);
    AddButton(mSwitchAxesMode);
  }

  void GizmoToolbar::SetMode(int aMode)
  {
    if (mMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<ObjectBrowser>()->GetCurrentObject() == nullptr)
    {
      aMode = Mode::Select;
    }

    switch (aMode)
    {
    case Mode::Select:
    {
      mSelect->setChecked(true);
      mSelect->ResetOtherButtons();
      SelectToggled(true);
      break;
    }

    case Mode::Translate:
    {
      mTranslate->setChecked(true);
      mTranslate->ResetOtherButtons();
      TranslateToggled(true);
      break;
    }

    case Mode::Rotate:
    {
      mRotate->setChecked(true);
      mRotate->ResetOtherButtons();
      RotateToggled(true);
      break;
    }

    case Mode::Scale:
    {
      mScale->setChecked(true);
      mScale->ResetOtherButtons();
      ScaleToggled(true);
      break;
    }
    }
  }

  void GizmoToolbar::SelectToggled(bool checked)
  {
    // if this button is now selected
    if (checked)
    {
      // set the gizmo mode to select
      static_cast<YTEditorMainWindow*>(mMainWindow)->GetGizmo()->SetOperation(Gizmo::Operation::Select);
    }
  }

  void GizmoToolbar::TranslateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      static_cast<YTEditorMainWindow*>(mMainWindow)->GetGizmo()->SetOperation(Gizmo::Operation::Translate);
    }
  }

  void GizmoToolbar::RotateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      static_cast<YTEditorMainWindow*>(mMainWindow)->GetGizmo()->SetOperation(Gizmo::Operation::Rotate);
    }
  }

  void GizmoToolbar::ScaleToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      static_cast<YTEditorMainWindow*>(mMainWindow)->GetGizmo()->SetOperation(Gizmo::Operation::Scale);
    }
  }

  void GizmoToolbar::SwitchAxesModeToggled(bool checked)
  {
    // if checked, it should be in local?
    if (checked)
    {
      static_cast<YTEditorMainWindow*>(mMainWindow)->GetGizmo()->SetMode(Gizmo::Mode::Local);
    }
    // otherwise, in world axes
    else
    {
      static_cast<YTEditorMainWindow*>(mMainWindow)->GetGizmo()->SetMode(Gizmo::Mode::World);
    }
  }

}