# Author: HenryAWE
# License: The 3-clause BSD License

include_guard(DIRECTORY)

find_package(PythonInterp REQUIRED)

function(tw_pack_files output basepath)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE}
        "${CMAKE_SOURCE_DIR}/tool/pack.py"
        "-o" ${output}
        "-b" ${basepath}
        "-i" ${ARGN}
    )
endfunction()
