# Author: HenryAWE
# License: The 3-clause BSD License

add_library(
    glad
    STATIC
    src/glad.c
)
target_include_directories(glad PUBLIC include)
