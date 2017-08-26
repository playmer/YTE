Set(Previous_YTE_Source ${YTE_Source})

AddFile(YTE_Source Previous_YTE_Source Events.cpp)
AddFile(YTE_Source Previous_YTE_Source Events.h)
AddFile(YTE_Source Previous_YTE_Source StandardEvents.cpp)
AddFile(YTE_Source Previous_YTE_Source StandardEvents.h)

Set(YTE_Source ${Previous_YTE_Source} PARENT_SCOPE)
