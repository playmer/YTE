#include <QPushButton>

#include "YTEditor/Framework/ForwardDeclarations.hpp"

namespace YTEditor
{
  namespace Framework
  {
    class ToolBarButton : public QPushButton
    {
    public:
      ToolBarButton(ToolBar* aToolbar, QString aIconPath, QString aActionName);

      void ResetOtherButtons();

      void SetResetterMode(bool isResetter);

      void SetIsUncheckable(bool isUncheckable);

      void SetIsResettable(bool isResettable);

      QString GetIcon()
      {
        return mIconPath;
      }

      QString GetActionName()
      {
        return mActionName;
      }

    private:
      void mousePressEvent(QMouseEvent* event) override;

      QString mIconPath;
      QString mActionName;
      ToolBar* mToolbar;

      bool mIsResetter;
      bool mIsResettable;
      bool mIsUncheckable;
    };
  }
}