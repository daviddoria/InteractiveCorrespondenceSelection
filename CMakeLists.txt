cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

PROJECT(InteractiveCorrespondenceSelection)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

QT4_WRAP_UI(UISrcs Form.ui)
QT4_WRAP_CPP(MOCSrcs Form.h)

ADD_EXECUTABLE(InteractiveCorrespondenceSelection
InteractiveCorrespondenceSelection.cpp
Form.cxx
Helpers.cpp
PointSelectionStyle2D.cpp
${UISrcs} ${MOCSrcs} ${ResourceSrcs})
TARGET_LINK_LIBRARIES(InteractiveCorrespondenceSelection QVTK ${VTK_LIBRARIES} ${ITK_LIBRARIES})

