cmake_minimum_required(VERSION 3.25.1)

project(Canavar VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INCLUDE_DIR libs/qtimgui/include 
				libs/assimp/include
				libs/JSBSim/include)
				
set(LIBS_DIR libs/qtimgui/lib
			 libs/assimp/lib
			 libs/JSBSim/lib)
			 
set(LIBS imgui 
		 implot 
		 qt_imgui_widgets 
		 assimp-vc142-mt 
		 zlibstatic)

link_directories(${LIBS_DIR})
include_directories(${INCLUDE_DIR})

add_subdirectory(projects/Engine Engine)
add_subdirectory(projects/Editor Editor)
add_subdirectory(projects/Simulator Simulator)