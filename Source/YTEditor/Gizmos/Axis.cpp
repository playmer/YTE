#include "../../YTE/Physics/Transform.hpp"

#include "Translate.hpp"
#include "Scale.hpp"
#include "Rotate.hpp"
#include "Axis.hpp"


Axis::Axis(int aDir, YTE::Engine * aEngine, YTE::Space * aSpace) 
  : YTE::Composition(aEngine, aSpace), mDir(aDir)
{

}