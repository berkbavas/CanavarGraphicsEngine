cmake_minimum_required(VERSION 3.25.1)

project(CanavarGraphicsEngineEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

add_compile_options(/bigobj)

file(GLOB SOURCES Src/*.cpp Include/*.h)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_executable(CanavarGraphicsEngineEditor ${SOURCES})

target_link_libraries(CanavarGraphicsEngineEditor CanavarGraphicsEngine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS})

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../../projects/Editor/resources\""
	"\"$(OutDir)resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../../projects/Editor/resources\""
	"\"$(BuildDir)resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../../projects/Engine/Resources\""
	"\"$(OutDir)resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../../projects/Engine/Resources\""
	"\"$(BuildDir)resources/*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../../libs/assimp/bin\""
	"\"$(OutDir)*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND xcopy /E /Y 
	"\"$(SolutionDir)../../libs/assimp/bin\""
	"\"$(BuildDir)*\""
)

add_custom_command(TARGET CanavarGraphicsEngineEditor POST_BUILD
    COMMAND $ENV{Qt6_DIR}/bin/windeployqt.exe 
	"\"$(OutDir)$(TargetName)$(TargetExt)\""
)