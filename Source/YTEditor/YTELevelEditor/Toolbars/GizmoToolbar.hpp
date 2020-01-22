#pragma once

#include "YTEditor/Framework/ToolBar.hpp"

namespace YTEditor
{
  class GizmoToolbar : public Framework::ToolBar
  {
  public:

    GizmoToolbar(Framework::MainWindow *aMainWindow);

    void SetMode(int aMode);

    enum Mode
    {
      Select,
      Translate,
      Rotate,
      Scale
    };

  private:

    Framework::ToolBarButton *mSelect;
    void SelectToggled(bool checked);

    Framework::ToolBarButton *mTranslate;
    void TranslateToggled(bool checked);

    Framework::ToolBarButton *mRotate;
    void RotateToggled(bool checked);

    Framework::ToolBarButton *mScale;
    void ScaleToggled(bool checked);

    Framework::ToolBarButton *mSwitchAxesMode;
    void SwitchAxesModeToggled(bool checked);
  };

}