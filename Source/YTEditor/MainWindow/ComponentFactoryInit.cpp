
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/MainWindow/ComponentFactoryInit.hpp"

namespace YTEditor
{
  void ComponentFactoryInitialization(YTE::Engine * aEngine, YTE::FactoryMap &currComponentFactories)
  {
    YTE::ComponentFactoryHelper helper(aEngine, &currComponentFactories);
  }
}