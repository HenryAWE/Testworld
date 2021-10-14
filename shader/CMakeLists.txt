# Author: HenryAWE
# License: The 3-clause BSD License

include(TWPack)
file(GLOB_RECURSE vs_files "*.vs")
file(GLOB_RECURSE fs_files "*.fs")
tw_pack_files(
    "${CMAKE_BINARY_DIR}/bin/shader.pak"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${vs_files} ${fs_files}
)
