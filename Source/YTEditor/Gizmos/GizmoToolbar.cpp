#include <filesystem>

#include <qdir.h>

#include "YTE/Core/AssetLoader.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/ToolbarButton.hpp"
#include "YTEditor/Gizmos/GizmoToolbar.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"

namespace YTEditor
{

  GizmoToolbar::GizmoToolbar(MainWindow * aMainWindow) : Toolbar(aMainWindow)
  {
    std::experimental::filesystem::path workingDir{ YTE::Path::GetEnginePath().String() };
    std::experimental::filesystem::path assetsDir{ workingDir.append("CreativeCommons_Icons/")};

    QString iconPath = (assetsDir.generic_string() + "select.png").c_str();
    mSelect = new ToolbarButton(this, iconPath);
    mSelect->connect(mSelect, &QPushButton::toggled, this, &GizmoToolbar::SelectToggled);

    iconPath = (assetsDir.generic_string() + "translate.png").c_str();
    mTranslate = new ToolbarButton(this, iconPath);
    mTranslate->connect(mTranslate, &QPushButton::toggled, this, &GizmoToolbar::TranslateToggled);

    iconPath = (assetsDir.generic_string() + "rotate.png").c_str();
    mRotate = new ToolbarButton(this, iconPath);
    mRotate->connect(mRotate, &QPushButton::toggled, this, &GizmoToolbar::RotateToggled);

    iconPath = (assetsDir.generic_string() + "scale.png").c_str();
    mScale = new ToolbarButton(this, iconPath);
    mScale->connect(mScale, &QPushButton::toggled, this, &GizmoToolbar::ScaleToggled);

    AddButton(mSelect);
    AddButton(mTranslate);
    AddButton(mRotate);
    AddButton(mScale);
  }

  void GizmoToolbar::SetMode(int aMode)
  {
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
      mMainWindow->GetGizmo()->SetMode(Gizmo::Select);
    }
  }

  void GizmoToolbar::TranslateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Translate);
    }
  }

  void GizmoToolbar::RotateToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Rotate);
    }
  }

  void GizmoToolbar::ScaleToggled(bool checked)
  {
    if (checked)
    {
      // set the gizmo mode to select
      mMainWindow->GetGizmo()->SetMode(Gizmo::Scale);
    }
  }

}