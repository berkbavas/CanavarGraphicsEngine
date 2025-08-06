cmake_minimum_required(VERSION 3.25.1)

project(Engine VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent OpenGLWidgets REQUIRED)

file(GLOB_RECURSE SOURCES Source/*.cpp Engine.qrc EngineBigObjects.qrc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_library(Engine STATIC ${SOURCES})

target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(Engine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent Qt6::OpenGLWidgets ${LIBS})