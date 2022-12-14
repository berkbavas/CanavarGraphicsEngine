cmake_minimum_required(VERSION 3.25.1)

project(CanavarGraphicsEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.4.1/msvc2019_64/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

add_compile_options(/bigobj)

file(GLOB SOURCES Src/*.cpp Include/*.h Src/*.qrc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_library(CanavarGraphicsEngine STATIC ${SOURCES})

target_include_directories(CanavarGraphicsEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_link_libraries(CanavarGraphicsEngine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS})