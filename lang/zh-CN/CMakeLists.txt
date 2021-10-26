# Author: HenryAWE
# License: The 3-clause BSD License

include(TWPack)
file(GLOB_RECURSE lang_files "*.json")
tw_pack_files(
    "${CMAKE_BINARY_DIR}/bin/lang/zh-CN.pak"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${lang_files}
    "${CMAKE_CURRENT_SOURCE_DIR}/SourceHanSansHWSC-Regular.otf"
)
