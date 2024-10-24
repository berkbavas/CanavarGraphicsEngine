cmake_minimum_required(VERSION 3.25.1)

project(CanavarGraphicsEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

add_compile_options(/bigobj)

file(GLOB_RECURSE SOURCES Source/*.cpp Source/*.h)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_executable(CanavarGraphicsEngine ${SOURCES})

target_include_directories(CanavarGraphicsEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")
target_link_libraries(CanavarGraphicsEngine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS})

add_custom_command(TARGET CanavarGraphicsEngine
    POST_BUILD COMMAND
    Qt6::windeployqt
    --dir "$<TARGET_FILE_DIR:CanavarGraphicsEngine>"
    "$<TARGET_FILE_DIR:CanavarGraphicsEngine>/$<TARGET_FILE_NAME:CanavarGraphicsEngine>")