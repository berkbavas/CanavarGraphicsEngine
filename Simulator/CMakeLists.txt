cmake_minimum_required(VERSION 3.25.1)

project(CanavarFlightSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

add_compile_options(/bigobj)

file(GLOB SOURCES Source/*.cpp Source/*.h)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_executable(CanavarFlightSimulator ${SOURCES})

target_link_libraries(CanavarFlightSimulator CanavarGraphicsEngine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS} JSBSim ws2_32)

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Simulator/Resources\""
	"\"$(OutDir)Resources/*\"")

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Simulator/Resources\""
	"\"$(BuildDir)Resources/*\"")

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Engine/Resources\""
	"\"$(OutDir)Resources/*\"")

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Engine/Resources\""
	"\"$(BuildDir)Resources/*\"")

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Libs/assimp/bin\""
	"\"$(OutDir)*\"")

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../Libs/assimp/bin\""
	"\"$(BuildDir)*\"")

add_custom_command(TARGET CanavarFlightSimulator POST_BUILD
    COMMAND $ENV{Qt6_DIR}/bin/windeployqt.exe 
	"\"$(OutDir)$(TargetName)$(TargetExt)\"")