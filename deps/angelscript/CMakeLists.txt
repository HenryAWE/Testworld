# Author: HenryAWE
# License: The 3-clause BSD License

find_package(Angelscript CONFIG REQUIRED)
add_library(
    as_addon
    STATIC
    scriptbuilder/scriptbuilder.cpp
    scriptstdstring/scriptstdstring.cpp
    scriptstdstring/scriptstdstring_utils.cpp
    scriptarray/scriptarray.cpp
)
target_include_directories(as_addon PUBLIC .)
target_link_libraries(as_addon PUBLIC Angelscript::angelscript)
if((${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
    target_link_libraries(as_addon PUBLIC Threads::Threads)
endif()
