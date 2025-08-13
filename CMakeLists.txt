cmake_minimum_required(VERSION 3.25.1)

project(Canavar VERSION 1.0.0 LANGUAGES CXX)

if(NOT MSVC)
    message(FATAL_ERROR "This project can only be built with Microsoft Visual C++ (MSVC).")
endif()

if(DEFINED ENV{Qt6_DIR})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/External")

set(QT_IMGUI_DIR    "${EXTERNAL_DIR}/qtimgui")
set(ASSIMP_DIR      "${EXTERNAL_DIR}/assimp")
set(JSBSIM_DIR      "${EXTERNAL_DIR}/JSBSim")
set(FREETYPE_DIR    "${EXTERNAL_DIR}/freetype")

add_library(EngineVendor INTERFACE)

target_include_directories(EngineVendor INTERFACE
    "${QT_IMGUI_DIR}/include"
    "${ASSIMP_DIR}/include"
    "${JSBSIM_DIR}/include"
    "${FREETYPE_DIR}/include/freetype2"
)

target_link_directories(EngineVendor INTERFACE
    "${QT_IMGUI_DIR}/lib"
    "${ASSIMP_DIR}/lib"
    "${JSBSIM_DIR}/lib"
    "${FREETYPE_DIR}/lib"
)

target_link_libraries(EngineVendor INTERFACE
    imgui
    qt_imgui_widgets
    assimp-vc143-mt
    zlibstatic
    JSBSim
    ws2_32
    freetype
)

function(configure_app_runtime TARGET_NAME)
  # Copy Assimp DLLs if present
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSIMP_DIR}/bin"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>"
  ) 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf
    "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/"
  ) 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/"
    "$<TARGET_FILE_DIR:${TARGET_NAME}>/Resources/"
  ) 
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND Qt6::windeployqt
            --dir "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/$<TARGET_FILE_NAME:${TARGET_NAME}>"
  )
endfunction()

add_subdirectory(Canavar/Engine Canavar/Engine)
add_subdirectory(Canavar/Editor Canavar/Editor)
add_subdirectory(Canavar/Simulator Canavar/Simulator)
add_subdirectory(Canavar/MultipleViews Canavar/MultipleViews)