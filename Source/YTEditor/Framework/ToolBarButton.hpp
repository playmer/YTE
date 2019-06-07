#include <qpushbutton.h>

#include "YTEditor/Framework/ForwardDeclarations.hpp"

namespace YTEditor
{
  namespace Framework
  {
    class ToolBarButton : public QPushButton
    {
    public:
      ToolBarButton(ToolBar* aToolbar, QString aIconPath);

      void ResetOtherButtons();

      void SetResetterMode(bool isResetter);

      void SetIsUncheckable(bool isUncheckable);

      void SetIsResettable(bool isResettable);

    private:

      bool mIsResetter;
      bool mIsResettable;

      bool mIsUncheckable;

      void mousePressEvent(QMouseEvent* event) override;

      ToolBar* mToolbar;

    };
  }
}