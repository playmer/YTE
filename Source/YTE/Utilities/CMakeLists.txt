Set(Previous_YTE_Source ${YTE_Source})

AddFile(YTE_Source Previous_YTE_Source JsonHelpers.h)
AddFile(YTE_Source Previous_YTE_Source Utilities.cpp)
AddFile(YTE_Source Previous_YTE_Source Utilities.h)
AddFile(YTE_Source Previous_YTE_Source DebugChecking/Assert.h)
AddFile(YTE_Source Previous_YTE_Source String/String.cpp)
AddFile(YTE_Source Previous_YTE_Source String/String.h)
AddFile(YTE_Source Previous_YTE_Source String/StringRange.cpp)
AddFile(YTE_Source Previous_YTE_Source String/StringRange.h)

Set(YTE_Source ${Previous_YTE_Source} PARENT_SCOPE)
