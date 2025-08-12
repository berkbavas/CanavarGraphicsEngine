cmake_minimum_required(VERSION 3.25.1)

project(Canavar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_IMGUI_DIR Libs/qtimgui)
set(ASSIMP_DIR Libs/assimp)
set(JSBSIM_DIR Libs/JSBSim)
set(FREETYPE_DIR Libs/freetype)

set(INCLUDE_DIR 
    ${QT_IMGUI_DIR}/include 
    ${ASSIMP_DIR}/include
    ${JSBSIM_DIR}/include
    ${FREETYPE_DIR}/include/freetype2
)
				
set(LIBS_DIR
    ${QT_IMGUI_DIR}/lib
    ${ASSIMP_DIR}/lib
    ${JSBSIM_DIR}/lib
    ${FREETYPE_DIR}/lib
)
			 
set(LIBS
    imgui
    qt_imgui_widgets
    assimp-vc143-mt
    zlibstatic
    JSBSim
    ws2_32
    freetype
)

link_directories(${LIBS_DIR})
include_directories(${INCLUDE_DIR})

add_subdirectory(Canavar/Engine Canavar/Engine)
add_subdirectory(Canavar/Editor Canavar/Editor)
add_subdirectory(Canavar/Simulator Canavar/Simulator)
add_subdirectory(Canavar/MultipleViews Canavar/MultipleViews)