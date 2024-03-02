cmake_minimum_required (VERSION 3.16)

add_library ("hibf_tutorial_lib" STATIC build.cpp count.cpp search.cpp)
target_link_libraries ("hibf_tutorial_lib" PUBLIC "${PROJECT_NAME}_interface")

add_executable ("hibf_tutorial" main.cpp)
target_link_libraries ("hibf_tutorial" PRIVATE "hibf_tutorial_lib")
