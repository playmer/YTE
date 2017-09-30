Set(Previous_YTE_Source ${YTE_Source})

AddFile(YTE_Source Previous_YTE_Source WWiseSystem.cpp)
AddFile(YTE_Source Previous_YTE_Source WWiseSystem.hpp)
AddFile(YTE_Source Previous_YTE_Source MemoryImplementation.cpp)
AddFile(YTE_Source Previous_YTE_Source WWiseEmitter.cpp)
AddFile(YTE_Source Previous_YTE_Source WWiseEmitter.hpp)
AddFile(YTE_Source Previous_YTE_Source WWiseListener.cpp)
AddFile(YTE_Source Previous_YTE_Source WWiseListener.hpp)
AddFile(YTE_Source Previous_YTE_Source Utilities.hpp)

Set(YTE_Source ${Previous_YTE_Source} PARENT_SCOPE)
