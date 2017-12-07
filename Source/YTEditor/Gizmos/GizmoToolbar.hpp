#pragma once

#include "YTEditor/MainWindow/Toolbar.hpp"

namespace YTEditor
{
  class MainWindow;
  class ToolbarButton;

  class GizmoToolbar : public Toolbar
  {
  public:

    GizmoToolbar(MainWindow *aMainWindow);

    void SetMode(int aMode);

    enum Mode
    {
      Select,
      Translate,
      Rotate,
      Scale
    };

  private:

    ToolbarButton *mSelect;
    void SelectToggled(bool checked);

    ToolbarButton *mTranslate;
    void TranslateToggled(bool checked);

    ToolbarButton *mRotate;
    void RotateToggled(bool checked);

    ToolbarButton *mScale;
    void ScaleToggled(bool checked);
  };

}