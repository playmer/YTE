#include <qpushbutton.h>

namespace YTEditor
{
  class GizmoToolbar;

  class GizmoButton : public QPushButton
  {
  public:
    GizmoButton(GizmoToolbar *aToolbar, QString aIconPath, void(GizmoToolbar::*onToggled)(bool));

  private:

    void mousePressEvent(QMouseEvent *event) override;

    GizmoToolbar *mToolbar;

  };

}