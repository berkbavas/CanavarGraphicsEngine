cmake_minimum_required(VERSION 3.25.1)

project(CanavarGraphicsEngine VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

file(GLOB_RECURSE SOURCES Source/*.cpp Source/*.h Engine.qrc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_library(CanavarGraphicsEngine STATIC ${SOURCES})

target_include_directories(CanavarGraphicsEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(CanavarGraphicsEngine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS})