#include "../../YTE/Core/Engine.hpp"

#include "../MainWindow/YTEditorMainWindow.hpp"

#include "Axis.hpp"
#include "Gizmo.hpp"

Gizmo::Gizmo(YTEditorMainWindow * aMainWindow) : mMode(Translate)
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
