project(Editor VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(Editor ${EDITOR_SOURCES})

target_include_directories(Editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(Editor 
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Engine
)

configure_app_runtime(Editor)