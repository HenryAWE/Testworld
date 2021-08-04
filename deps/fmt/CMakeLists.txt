# Author: HenryAWE
# License: The 3-clause BSD License

add_library(
    fmt
    STATIC
    src/format.cc
)
target_include_directories(fmt PUBLIC include)
