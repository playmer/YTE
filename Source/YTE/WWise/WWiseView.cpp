
#include "YTE/WWise/WWiseView.hpp"


namespace YTE
{
  YTEDefineEvent(WWiseListenerChanged);

  YTEDefineType(WWiseListenerChanged)
  {
    RegisterType<WWiseListenerChanged>();
  }

  YTEDefineType(WWiseView)
  {
    RegisterType<WWiseView>();

    GetStaticType()->AddAttribute<RunInEditor>();
  }
}