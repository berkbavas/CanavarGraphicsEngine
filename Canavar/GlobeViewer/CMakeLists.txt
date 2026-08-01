project(GlobeViewer VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE GV_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(GlobeViewer ${GV_SOURCES})

set(TERRAIN_CORE_DIR "${CMAKE_SOURCE_DIR}/Canavar/TerrainCore")

target_include_directories(GlobeViewer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${TERRAIN_CORE_DIR}/Include"
)

target_link_libraries(GlobeViewer
  PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    TerrainCore
    EngineVendor
)

configure_app_runtime(GlobeViewer)
