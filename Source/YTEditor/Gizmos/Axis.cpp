#include "YTE/Physics/Transform.hpp"

#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/Gizmos/Axis.hpp"

namespace YTEditor
{

  Axis::Axis(int aDir, YTE::Engine * aEngine, YTE::Space * aSpace)
    : YTE::Composition(aEngine, aSpace), mDir(aDir)
  {

  }

}
