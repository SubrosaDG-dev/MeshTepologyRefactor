# function to message tool version
function(message_tool_version toolname toolpath)
    execute_process(
        COMMAND ${toolpath} --version
        RESULT_VARIABLE _
        OUTPUT_VARIABLE _out
    )
    string(REGEX REPLACE "\n" ";" _out "${_out}")
    string(REGEX MATCHALL "([^;]+;|[^;]+$)" _out_list "${_out}")
    list(GET _out_list 0 _output)
    message(STATUS "Found ${toolname}: ${_output}")
endfunction()
