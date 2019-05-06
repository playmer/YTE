#include "YTE/WWise/WWiseView.hpp"


namespace YTE
{
  YTEDefineEvent(WWiseListenerChanged);

  YTEDefineType(WWiseListenerChanged)
  {
    RegisterType<WWiseListenerChanged>();
    TypeBuilder<WWiseListenerChanged> builder;
  }

  YTEDefineType(WWiseView)
  {
    RegisterType<WWiseView>();
    TypeBuilder<WWiseView> builder;

    GetStaticType()->AddAttribute<RunInEditor>();
  }
}