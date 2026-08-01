project(DtedDownloader VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE DTED_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(DtedDownloader ${DTED_SOURCES})

target_include_directories(DtedDownloader PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(DtedDownloader
  PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Network
    EngineVendor
)

configure_app_runtime(DtedDownloader)
