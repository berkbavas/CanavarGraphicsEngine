cmake_minimum_required(VERSION 3.25.1)

project(Editor VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Widgets OpenGL Gui Concurrent REQUIRED)

file(GLOB_RECURSE SOURCES Source/*.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source" ${INCLUDE_DIR})

link_directories(${LIBS_DIR})

add_executable(Editor ${SOURCES})

target_link_libraries(Editor Engine Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent ${LIBS})

add_custom_command(TARGET Editor
    POST_BUILD COMMAND ${CMAKE_COMMAND}
    -E rm -rf
    "$<TARGET_FILE_DIR:Editor>/Resources/"
)

add_custom_command(TARGET Editor
    POST_BUILD COMMAND ${CMAKE_COMMAND}
    -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/"
    "$<TARGET_FILE_DIR:Editor>/Resources/"
)

add_custom_command(TARGET Editor
    POST_BUILD COMMAND ${CMAKE_COMMAND} 
    -E copy_directory 
    "${CMAKE_SOURCE_DIR}/${ASSIMP_DIR}/bin/" 
    "$<TARGET_FILE_DIR:Editor>"
)

add_custom_command(TARGET Editor
    POST_BUILD COMMAND
    Qt6::windeployqt
    --dir "$<TARGET_FILE_DIR:Editor>"
    "$<TARGET_FILE_DIR:Editor>/$<TARGET_FILE_NAME:Editor>"
)