# Author: HenryAWE
# License: The 3-clause BSD License

# External packages
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
endif()
find_package(SDL2 REQUIRED)
find_package(Angelscript CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(PhysFS REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

# Source list
# Program entry
aux_source_directory(. testworld_src)
# Editor
aux_source_directory(editor testworld_editor_src)
list(APPEND testworld_src ${testworld_editor_src})

# Graphic
aux_source_directory(graphic testworld_graphic_src)
list(APPEND testworld_src ${testworld_graphic_src})
aux_source_directory(graphic/common testworld_common_src)
list(APPEND testworld_src ${testworld_common_src})
aux_source_directory(graphic/opengl3 testworld_opengl3_src)
list(APPEND testworld_src ${testworld_opengl3_src})
# Resource management
aux_source_directory(res testworld_res_src)
list(APPEND testworld_src ${testworld_res_src})
aux_source_directory(res/lang testworld_lang_src)
list(APPEND testworld_src ${testworld_lang_src})
# Scripting
aux_source_directory(script testworld_script_src)
list(APPEND testworld_src ${testworld_script_src})
# System
aux_source_directory(sys testworld_sys_src)
list(APPEND testworld_src ${testworld_sys_src})
# Command line interface
aux_source_directory(cli testworld_cli_src)
list(APPEND testworld_src ${testworld_cli_src})
# User interface
aux_source_directory(ui testworld_ui_src)
list(APPEND testworld_src ${testworld_ui_src})
# Window
aux_source_directory(window testworld_window_src)
list(APPEND testworld_src ${testworld_window_src})

add_executable(
    testworld
    WIN32
    ${testworld_src}
)

# Compiler opetions
if(MSVC)
    target_compile_definitions(testworld PRIVATE _CRT_SECURE_NO_WARNINGS=1)
    target_compile_definitions(testworld PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS=1)
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(testworld PRIVATE stdc++fs)
    target_compile_options(testworld PRIVATE -Wno-format-security)
endif()

target_link_libraries(testworld PRIVATE SDL2::Core SDL2::Entry)
target_link_libraries(testworld PRIVATE Angelscript::angelscript)
target_link_libraries(testworld PRIVATE freetype)
target_link_libraries(testworld PRIVATE Boost::program_options)
target_link_libraries(testworld PRIVATE as_addon)
target_link_libraries(testworld PRIVATE ${PHYSFS_LIBRARY})
target_include_directories(testworld PRIVATE ${PHYSFS_INCLUDE_DIR})
target_link_libraries(testworld PRIVATE fmt)
target_link_libraries(testworld PRIVATE glad)
target_link_libraries(testworld PRIVATE glm)
target_link_libraries(testworld PRIVATE imgui)
target_link_libraries(testworld PRIVATE rapidjson)
target_link_libraries(testworld PRIVATE stb)

set_target_properties(
    testworld
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
)
