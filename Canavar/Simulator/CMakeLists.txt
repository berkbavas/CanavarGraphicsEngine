project(Simulator VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE SIMULATOR_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(Simulator ${SIMULATOR_SOURCES})

target_include_directories(Simulator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(Simulator
  PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Engine
)

configure_app_runtime(Simulator)