#include "YTE/Lua/LuaLibraryInclude.hpp"
#include "YTE/Lua/LuaScriptLoader.hpp"

namespace YTE
{
  void LuaScriptLoader::SetScriptPath(const std::experimental::filesystem::path& aScriptPath)
  {
    mScriptPath = aScriptPath;
  }

  int LuaScriptLoader::GetScriptReference(lua_State *aL, const std::string& aFilename)
  {
    auto it = mScriptReferences.find(aFilename);
    if (it != mScriptReferences.end())
    {
      return it->second;
    }

    int result = luaL_loadfile(aL, (mScriptPath / aFilename).string().c_str());
  }
}