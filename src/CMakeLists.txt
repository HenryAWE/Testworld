# Author: HenryAWE
# License: The 3-clause BSD License

find_package(SDL2 CONFIG REQUIRED)
find_package(Angelscript CONFIG REQUIRED)
find_package(PhysFS REQUIRED)

aux_source_directory(editor testworld_editor_src)
aux_source_directory(renderer testworld_renderer_src)
aux_source_directory(res testworld_res_src)
aux_source_directory(sys testworld_sys_src)
aux_source_directory(window testworld_window_src)
aux_source_directory(. testworld_src)
add_executable(
    testworld
    WIN32
    ${testworld_editor_src}
    ${testworld_renderer_src}
    ${testworld_res_src}
    ${testworld_sys_src}
    ${testworld_window_src}
    ${testworld_src}
)

target_compile_definitions(testworld PRIVATE _CRT_SECURE_NO_WARNINGS=1)

target_link_libraries(testworld PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(testworld PRIVATE Angelscript::angelscript)
target_link_libraries(testworld PRIVATE as_addon)
target_link_libraries(testworld PRIVATE ${PHYSFS_LIBRARY})
target_include_directories(testworld PRIVATE ${PHYSFS_INCLUDE_DIR})
target_link_libraries(testworld PRIVATE fmt)
target_link_libraries(testworld PRIVATE glad)
target_link_libraries(testworld PRIVATE glm)
target_link_libraries(testworld PRIVATE imgui)
target_link_libraries(testworld PRIVATE stb)

set_target_properties(
    testworld
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
)
