function(gb2gc_add_binary_dir_copy TARGET FILENAME)
    add_custom_command(TARGET gb2gc_copy_test_data
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
        VERBATIM
    )
endfunction()