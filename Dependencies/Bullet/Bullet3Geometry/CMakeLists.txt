Set(Previous_Bullet_Source ${Bullet_Source})

AddFile(Bullet_Source Previous_Bullet_Source b3AabbUtil.h)
AddFile(Bullet_Source Previous_Bullet_Source b3ConvexHullComputer.cpp)
AddFile(Bullet_Source Previous_Bullet_Source b3ConvexHullComputer.h)
AddFile(Bullet_Source Previous_Bullet_Source b3GeometryUtil.cpp)
AddFile(Bullet_Source Previous_Bullet_Source b3GeometryUtil.h)
AddFile(Bullet_Source Previous_Bullet_Source b3GrahamScan2dConvexHull.h)
AddFile(Bullet_Source Previous_Bullet_Source premake4.lua)

Set(Bullet_Source ${Previous_Bullet_Source} PARENT_SCOPE)
