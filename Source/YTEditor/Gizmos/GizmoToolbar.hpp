#include <qtoolbar.h>


namespace YTEditor
{
  class MainWindow;

  class GizmoToolbar : public QToolBar
  {
  public:

    GizmoToolbar(MainWindow *aMainWindow);


  private:

    MainWindow *mMainWindow;

    QPushButton *mSelect;
    QPushButton *mTranslate;
    QPushButton *mRotate;
    QPushButton *mScale;

    void SelectToggled(bool checked);
    void TranslateToggled(bool checked);
    void RotateToggled(bool checked);
    void ScaleToggled(bool checked);


  };

}