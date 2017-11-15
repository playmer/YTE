Set(Previous_YTE_Source ${YTE_Source})

AddFile(YTE_Source Previous_YTE_Source LuaSystem.hpp)
AddFile(YTE_Source Previous_YTE_Source LuaSystem.cpp)
AddFile(YTE_Source Previous_YTE_Source LuaComponent.hpp)
AddFile(YTE_Source Previous_YTE_Source LuaComponent.cpp)
AddFile(YTE_Source Previous_YTE_Source LuaScriptLoader.hpp)
AddFile(YTE_Source Previous_YTE_Source LuaScriptLoader.cpp)
AddFile(YTE_Source Previous_YTE_Source LuaLibraryInclude.hpp)

Set(YTE_Source ${Previous_YTE_Source} PARENT_SCOPE)
