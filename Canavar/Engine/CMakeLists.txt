project(Engine VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS Source/*.cpp Engine.qrc EngineBigObjects.qrc)

add_library(Engine STATIC ${ENGINE_SOURCES})

target_include_directories(Engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source" )

target_link_libraries(Engine 
  PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    EngineVendor
)