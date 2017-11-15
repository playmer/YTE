#pragma once

#include <experimental/filesystem>
#include <map>

struct lua_State;

namespace YTE
{
  class LuaScriptLoader
  {
  public:
    void SetScriptPath(const std::experimental::filesystem::path& aScriptPath);
    int GetScriptReference(lua_State *aL, const std::string& aFilename);

  private:
    std::experimental::filesystem::path mScriptPath;
    std::map<std::string, int> mScriptReferences;
  };
}