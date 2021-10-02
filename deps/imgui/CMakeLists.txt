# Author: HenryAWE
# License: The 3-clause BSD License

find_package(SDL2 REQUIRED)

aux_source_directory(. imgui_src)
add_library(
    imgui
    STATIC
    ${imgui_src}
)
target_include_directories(imgui PUBLIC .)
if(CMAKE_COMPILER_IS_GNUCXX)
    target_include_directories(imgui PRIVATE ${SDL2_INCLUDE_DIR})
else()
    target_link_libraries(imgui PRIVATE SDL2::Core)
endif()
target_link_libraries(imgui PRIVATE glad)
target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_compile_definitions(imgui PRIVATE _CRT_SECURE_NO_WARNINGS)
