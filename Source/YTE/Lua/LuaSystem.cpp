#include "YTE/Lua/LuaSystem.hpp"

#include "YTE/Lua/LuaLibraryInclude.hpp"

namespace YTE
{
  YTEDefineType(LuaSystem)
  {
    YTERegisterType(LuaSystem);
  }

  LuaSystem::LuaSystem(Composition *aOwner, RSValue *aProperties) : Component(aOwner, nullptr)
  {
  }

  void LuaSystem::Initialize()
  {
    mL = luaL_newstate();
    luaL_openlibs(mL);
  }
  void LuaSystem::Update(float)
  {
  }

  lua_State *LuaSystem::GetLuaState()
  {
    return mL;
  }

  int LuaSystem::LoadLuaScript(const std::string& aFilename)
  {
    // TODO(Austin): Do it!
    return 0;
  }

  LuaSystem::~LuaSystem()
  {
  }


}

int indent = 0;
int tableRec = 0;

__declspec(dllexport) void PrintIndent()
{
  for (int i = 0; i < indent; ++i)
  {
    printf(" ");
  }
}

__declspec(dllexport) void Lua_PrintIndex(lua_State *L, int index)
{
  int t = lua_type(L, index);

  switch (t)
  {
  case LUA_TSTRING:  /* strings */
    printf("%s", lua_tostring(L, index));
    break;

  case LUA_TBOOLEAN:  /* booleans */
    printf("%s", lua_toboolean(L, index) != 0 ? "true" : "false");
    break;

  case LUA_TNUMBER:  /* numbers */
  {
    double number = lua_tonumber(L, index);
    printf("%f", static_cast<float>(number));
  }
  break;

  case LUA_TTABLE:
  {
    Lua_PrintTable(L, index);
  }
  break;

  case LUA_TFUNCTION:
  {
    printf("[Function]: %p", (void *)lua_tocfunction(L, index));
  }
  break;

  case LUA_TLIGHTUSERDATA:
  {
    printf("[Light Userdata]: %p", lua_touserdata(L, index));
  }
  break;

  case LUA_TUSERDATA:
  {
    // lua_getmetatable(L, index);
    // lua_pushstring(L, "__print");
    // lua_rawget(L, -2);
    //if (lua_isfunction(L, -1))
    //{
    //  lua_pushvalue(L, index);
    //  lua_call(L, 1, 1);
    //}
    //else
    {
      //  lua_pop(L, 1);
      printf("[Userdata]: %p\n", *(void **)lua_touserdata(L, index));
    }
    printf("\n");
    //lua_pop(L, 1);
  }
  break;

  case LUA_TTHREAD:
  {
    printf("[Thread]: %p\n", (void *)lua_tothread(L, index));
  }
  break;

  case LUA_TNIL:
  {
    printf("nil");
  }
  break;

  default:  /* other values */
    printf("dumpStack Err, type encountered: %s", lua_typename(L, t));
    /*Nothing to add to the datapair*/
    break;
  }
}

__declspec(dllexport) void Lua_PrintTable(lua_State *L, int index)
{
  (void)L, (void)index;

  printf("Table: \n");

  PrintIndent(); printf("{\n");
  indent += 2;

  lua_pushvalue(L, index);
  lua_pushnil(L);  // first key

  while (lua_next(L, -2) != 0)
  {
    // uses 'key' (at index -2) and 'value' (at index -1)
    lua_pushvalue(L, -2);
    //const char *key = lua_tostring(L, -1);
    PrintIndent(); printf("Key: "); Lua_PrintIndex(L, -1); printf("\n");
    //PrintIndent(); printf("Value: "); Lua_PrintIndex(L, -1); printf("\n");

    // removes 'value'; keeps 'key' for next iteration
    lua_pop(L, 2);
  }

  lua_pop(L, 1);

  indent -= 2;
  PrintIndent(); printf("}");
}
__declspec(dllexport) void Lua_stackDump(lua_State *L)
{

  //if (doPrint == 0)
  //return;

  printf("--- LUA TOP ---\n");
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++)
  {  /* repeat for each element on the stack */
    tableRec = 0;
    printf("%i - ", i); Lua_PrintIndex(L, i);
    printf("\n");
  }

  printf("--- END ---\n\n");
}

__declspec(dllexport) void Lua_stackTrace(lua_State *L)
{
  luaL_traceback(L, L, nullptr, 0);
  const char *error = lua_tostring(L, -1);
  printf("%s\n", error);

  lua_pop(L, 1);
}

void LuaStackTrace(lua_State* L)
{
  Lua_stackTrace(L);
}

void LuaStackDump(lua_State* L)
{
  Lua_stackDump(L);
}