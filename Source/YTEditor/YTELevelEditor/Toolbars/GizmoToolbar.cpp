#include "YTE/StandardLibrary/FileSystem.hpp"

#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/Framework/MainWindow.hpp"

#include "YTEditor/Framework/ToolBarButton.hpp"

#include "YTEditor/YTELevelEditor/Toolbars/GizmoToolbar.hpp"

#include "YTEditor/YTELevelEditor/Widgets/CompositionBrowser/CompositionBrowser.hpp"

#include "YTEditor/YTELevelEditor/Gizmo.hpp"
#include "YTEditor/YTELevelEditor/YTEditorMainWindow.hpp"
#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{

  GizmoToolbar::GizmoToolbar(Framework::MainWindow* aMainWindow)
    : Framework::ToolBar(aMainWindow)
  {
    std::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
    std::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/")};

    QString iconPath = (assetsDir.generic_string() + "select.png").c_str();
    mSelect = new Framework::ToolBarButton(this, iconPath, "Select Gizmo");
    mSelect->connect(mSelect, &QPushButton::toggled, this, &GizmoToolbar::SelectToggled);

    iconPath = (assetsDir.generic_string() + "translate.png").c_str();
    mTranslate = new Framework::ToolBarButton(this, iconPath, "Translate Gizmo");
    mTranslate->connect(mTranslate, &QPushButton::toggled, this, &GizmoToolbar::TranslateToggled);

    iconPath = (assetsDir.generic_string() + "rotate.png").c_str();
    mRotate = new Framework::ToolBarButton(this, iconPath, "Rotate Gizmo");
    mRotate->connect(mRotate, &QPushButton::toggled, this, &GizmoToolbar::RotateToggled);

    iconPath = (assetsDir.generic_string() + "scale.png").c_str();
    mScale = new Framework::ToolBarButton(this, iconPath, "Scale Gizmo");
    mScale->connect(mScale, &QPushButton::toggled, this, &GizmoToolbar::ScaleToggled);

    iconPath = (assetsDir.generic_string() + "switchAxes.png").c_str();
    mSwitchAxesMode = new Framework::ToolBarButton(this, iconPath, "Switch Gizmo Axes");
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
    if (mMainWindow->GetWorkspace<YTELevelEditor>()->GetWidget<CompositionBrowser>()->GetCurrentObject() == nullptr)
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
      mMainWindow->GetWorkspace<YTELevelEditor>()->GetGizmo()->SetOperation(Gizmo::Operation::Select);
    }
  }

  void GizmoToolbar::TranslateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetWorkspace<YTELevelEditor>()->GetGizmo()->SetOperation(Gizmo::Operation::Translate);
    }
  }

  void GizmoToolbar::RotateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetWorkspace<YTELevelEditor>()->GetGizmo()->SetOperation(Gizmo::Operation::Rotate);
    }
  }

  void GizmoToolbar::ScaleToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetWorkspace<YTELevelEditor>()->GetGizmo()->SetOperation(Gizmo::Operation::Scale);
    }
  }

  void GizmoToolbar::SwitchAxesModeToggled(bool checked)
  {
    // if checked, it should be in local?
    if (checked)
    {
      mMainWindow->GetWorkspace<YTELevelEditor>()->GetGizmo()->SetMode(Gizmo::Mode::Local);
    }
    // otherwise, in world axes
    else
    {
      mMainWindow->GetWorkspace<YTELevelEditor>()->GetGizmo()->SetMode(Gizmo::Mode::World);
    }
  }

}