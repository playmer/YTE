target_sources(Bullet
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/b3ConvexHullComputer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/b3GeometryUtil.cpp
#  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/b3AabbUtil.h
    ${CMAKE_CURRENT_LIST_DIR}/b3ConvexHullComputer.h
    ${CMAKE_CURRENT_LIST_DIR}/b3GeometryUtil.h
    ${CMAKE_CURRENT_LIST_DIR}/b3GrahamScan2dConvexHull.h
)
