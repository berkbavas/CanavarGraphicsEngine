project(ImageryDownloader VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE IMAGERY_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(ImageryDownloader ${IMAGERY_SOURCES})

target_include_directories(ImageryDownloader PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(ImageryDownloader
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Network
    EngineVendor
)

configure_app_runtime(ImageryDownloader)
