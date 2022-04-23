##########################################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
##########################################################################################

function(ltb_create_app target)
    ltb_add_executable(
            ${target}
            ${ARGN}
    )
    ltb_target_link_libraries(
            ${target}
            PRIVATE
            LtbApp::LtbApp
    )
    ltb_target_compile_features(
            ${target}
            PRIVATE
            cxx_std_17
    )
endfunction()
