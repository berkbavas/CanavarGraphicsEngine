project(TerrainCore VERSION 1.0.0 LANGUAGES CXX)

add_library(TerrainCore STATIC
    Source/QuadtreeIndex.cpp
)

target_include_directories(TerrainCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")

target_link_libraries(TerrainCore PUBLIC Qt6::Core)
