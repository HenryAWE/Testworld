# The Main CMakeLists.txt of the Project Testworld
#
# Author: HenryAWE
# License: The 3-clause BSD License

include(TWPack)
set(
    res_files
    "${CMAKE_CURRENT_SOURCE_DIR}/awesomeface.png"
)
tw_pack_files(
    "${CMAKE_BINARY_DIR}/bin/resource.pak"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${res_files}
)
