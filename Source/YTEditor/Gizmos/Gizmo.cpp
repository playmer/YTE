#include "../../YTE/Core/Engine.hpp"

#include "../MainWindow/YTEditorMainWindow.hpp"

#include "Axis.hpp"
#include "Gizmo.hpp"

Gizmo::Gizmo(YTEditorMainWindow * aMainWindow)
{
  YTE::Engine *engine = aMainWindow->GetRunningEngine();
  YTE::Space *space = engine->GetSpace();
    
  // create 3 axes
  mX = new Axis(Axis::X, engine, space);
  mY = new Axis(Axis::Y, engine, space);
  mZ = new Axis(Axis::Z, engine, space);

  auto *compositions = space->GetCompositions();
  compositions->Emplace("Gizmo X Axis", mX);
  compositions->Emplace("Gizmo Y Axis", mY);
  compositions->Emplace("Gizmo Z Axis", mZ);
}

bool Gizmo::IsAxis(YTE::Composition *aObject)
{
  if (aObject == reinterpret_cast<YTE::Composition*>(mX))
  {
    return true;
  }
  else if (aObject == reinterpret_cast<YTE::Composition*>(mY))
  {
    return true;
  }
  else if (aObject == reinterpret_cast<YTE::Composition*>(mZ))
  {
    return true;
  }

  return false;
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
