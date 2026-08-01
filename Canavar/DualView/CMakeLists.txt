project(DualView VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE DUALVIEW_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(DualView ${DUALVIEW_SOURCES})

target_include_directories(DualView PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(DualView
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Engine
)

configure_app_runtime(DualView)
