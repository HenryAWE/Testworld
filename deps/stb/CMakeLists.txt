# Author: HenryAWE
# License: The 3-clause BSD License

add_library(
    stb
    STATIC
    compile.c
)
target_include_directories(stb PUBLIC .)
