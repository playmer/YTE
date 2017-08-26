Set(Bullet_Source)

Get_Filename_Component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
Add_Subdirectory(Bullet3Collision)
Add_Subdirectory(Bullet3Common)
Add_Subdirectory(Bullet3Dynamics)
Add_Subdirectory(Bullet3Geometry)
Add_Subdirectory(Bullet3OpenCL)
Add_Subdirectory(Bullet3Serialize)
Add_Subdirectory(BulletCollision)
Add_Subdirectory(BulletDynamics)
Add_Subdirectory(BulletInverseDynamics)
Add_Subdirectory(BulletSoftBody)
Add_Subdirectory(clew)
Add_Subdirectory(LinearMath)
Include_Directories(${ParentDirectory} ${CMAKE_CURRENT_SOURCE_DIR})

Set(Bullet_Source ${Bullet_Source} 
        btBulletCollisionCommon.h
        btBulletDynamicsCommon.h)


Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DB3_USE_CLEW")

Add_Library(Bullet ${Bullet_Source})