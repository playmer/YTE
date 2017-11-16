#include <qpushbutton.h>

namespace YTEditor
{
  class Toolbar;

  class ToolbarButton : public QPushButton
  {
  public:
    ToolbarButton(Toolbar *aToolbar, QString aIconPath);

    void SetResetterMode(bool isResetter);

    void SetIsUncheckable(bool isUncheckable);

  private:

    bool mIsResetter;
    bool mIsUncheckable;

    void mousePressEvent(QMouseEvent *event) override;

    Toolbar *mToolbar;

  };

}