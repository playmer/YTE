#include <qtoolbar.h>


namespace YTEditor
{
  class MainWindow;
  class GizmoButton;

  class GizmoToolbar : public QToolBar
  {
  public:

    GizmoToolbar(MainWindow *aMainWindow);

    std::vector<GizmoButton*>& GetButtons();

  private:

    MainWindow *mMainWindow;

    std::vector<GizmoButton*> mButtons;

    GizmoButton *mSelect;
    GizmoButton *mTranslate;
    GizmoButton *mRotate;
    GizmoButton *mScale;

    void SelectToggled(bool checked);
    void TranslateToggled(bool checked);
    void RotateToggled(bool checked);
    void ScaleToggled(bool checked);

  };

}