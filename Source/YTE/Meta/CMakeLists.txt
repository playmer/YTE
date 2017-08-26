Set(Previous_YTE_Source ${YTE_Source})

AddFile(YTE_Source Previous_YTE_Source Attribute.cpp)
AddFile(YTE_Source Previous_YTE_Source Attribute.hpp)
AddFile(YTE_Source Previous_YTE_Source Field.hpp)
AddFile(YTE_Source Previous_YTE_Source ForwardDeclarations.hpp)
AddFile(YTE_Source Previous_YTE_Source Function.hpp)
AddFile(YTE_Source Previous_YTE_Source Property.hpp)
AddFile(YTE_Source Previous_YTE_Source Reflection.hpp)
AddFile(YTE_Source Previous_YTE_Source Type.cpp)
AddFile(YTE_Source Previous_YTE_Source Type.hpp)

Set(YTE_Source ${Previous_YTE_Source} PARENT_SCOPE)
