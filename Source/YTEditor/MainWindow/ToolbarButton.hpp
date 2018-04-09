#include <qpushbutton.h>

namespace YTEditor
{
  class Toolbar;

  class ToolbarButton : public QPushButton
  {
  public:
    ToolbarButton(Toolbar *aToolbar, QString aIconPath);

    void ResetOtherButtons();

    void SetResetterMode(bool isResetter);

    void SetIsUncheckable(bool isUncheckable);

    void SetIsResettable(bool isResettable);

  private:

    bool mIsResetter;
    bool mIsResettable;

    bool mIsUncheckable;

    void mousePressEvent(QMouseEvent *event) override;

    Toolbar *mToolbar;

  };

}