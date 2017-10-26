#include "YTE/Core/Engine.hpp"

#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"

namespace YTEditor
{

  Gizmo::Gizmo(MainWindow * aMainWindow) : mMainWindow(aMainWindow), mMode(Translate)
  {
  }

  void Gizmo::SetMode(int aMode)
  {
    if (Select <= aMode && aMode <= Rotate)
    {
      mMode = aMode;
    }
  }

  int Gizmo::GetCurrentMode()
  {
    return mMode;
  }

}
