cmake_minimum_required(VERSION 3.25.1)

project(Canavar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INCLUDE_DIR Libs/qtimgui/include 
                Libs/assimp/include
                Libs/JSBSim/include
)
				
set(LIBS_DIR    Libs/qtimgui/lib
                Libs/assimp/lib
                Libs/JSBSim/lib
)
			 
set(LIBS    imgui
            qt_imgui_widgets
            assimp-vc143-mt
            zlibstatic
)

link_directories(${LIBS_DIR})
include_directories(${INCLUDE_DIR})

add_subdirectory(Canavar/Engine Canavar/Engine)
