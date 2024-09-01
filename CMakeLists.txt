cmake_minimum_required(VERSION 3.16)
project(HeightMap)

find_package(VTK REQUIRED)

add_executable(HeightMap HeightMap.cpp)

target_link_libraries(HeightMap PRIVATE ${VTK_LIBRARIES})

vtk_module_autoinit(
    TARGETS HeightMap
    MODULES ${VTK_LIBRARIES}
)
