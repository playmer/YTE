get_filename_component(parentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

add_library(Bullet 
  btBulletCollisionCommon.h
  btBulletDynamicsCommon.h
) 

target_include_directories(Bullet PUBLIC ${parentDirectory} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(Bullet PUBLIC B3_USE_CLEW)

include(Bullet3Collision/CMakeLists.txt)
include(Bullet3Common/CMakeLists.txt)
include(Bullet3Dynamics/CMakeLists.txt)
include(Bullet3Geometry/CMakeLists.txt)
include(Bullet3OpenCL/CMakeLists.txt)
include(Bullet3Serialize/CMakeLists.txt)
include(BulletCollision/CMakeLists.txt)
include(BulletDynamics/CMakeLists.txt)
include(BulletInverseDynamics/CMakeLists.txt)
include(BulletSoftBody/CMakeLists.txt)
include(clew/CMakeLists.txt)
include(LinearMath/CMakeLists.txt)

YTE_Source_Group(CMAKE_CURRENT_SOURCE_DIR Bullet) 
