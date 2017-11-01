Set(Previous_YTE_Source ${YTE_Source})

AddFile(YTE_Source Previous_YTE_Source Class.hpp)
AddFile(YTE_Source Previous_YTE_Source Class.cpp)
AddFile(YTE_Source Previous_YTE_Source Namespace.hpp)
AddFile(YTE_Source Previous_YTE_Source Function.hpp)

Set(YTE_Source ${Previous_YTE_Source} PARENT_SCOPE)
