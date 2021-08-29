# Author: HenryAWE
# License: The 3-clause BSD License

find_package(SDL2 CONFIG REQUIRED)

aux_source_directory(. imgui_src)
add_library(
    imgui
    STATIC
    ${imgui_src}
)
target_include_directories(imgui PUBLIC .)
target_link_libraries(imgui PRIVATE SDL2::SDL2)
target_link_libraries(imgui PRIVATE glad)
target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_compile_definitions(imgui PRIVATE _CRT_SECURE_NO_WARNINGS)
