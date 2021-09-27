# Author: HenryAWE
# License: The 3-clause BSD License

include(TWPack)
file(GLOB_RECURSE files "*.as")
tw_pack_files(
    "${CMAKE_BINARY_DIR}/bin/script.pak"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${files}
)
