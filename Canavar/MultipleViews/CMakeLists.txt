project(MultipleViews VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE MULTIPLE_VIEWS_SOURCES CONFIGURE_DEPENDS Source/*.cpp)

add_executable(MultipleViews ${MULTIPLE_VIEWS_SOURCES})

target_include_directories(MultipleViews PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(MultipleViews PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Engine)

configure_app_runtime(MultipleViews)