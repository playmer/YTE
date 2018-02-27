
#include "YTE/WWise/WWiseView.hpp"


namespace YTE
{
  YTEDefineEvent(WWiseListenerChanged);

  YTEDefineType(WWiseListenerChanged)
  {
    YTERegisterType(WWiseListenerChanged);
  }

  YTEDefineType(WWiseView)
  {
    YTERegisterType(WWiseView);

    GetStaticType()->AddAttribute<RunInEditor>();
  }
}