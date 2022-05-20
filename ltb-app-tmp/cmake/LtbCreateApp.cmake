# ##############################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
# ##############################################################################

function(ltb_create_app target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target}
                          PRIVATE LtbApp::LtbApp
                                  doctest::doctest
                                  $<TARGET_NAME_IF_EXISTS:ltb_dev_settings>
                                  $<$<BOOL:${LTB_LINK_GCOV}>:gcov>
                          )
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_definitions(${target}
                               PRIVATE
                                   $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
                                   $<$<PLATFORM_ID:Windows>:NOMINMAX>
                                   $<$<NOT:$<BOOL:${LTB_ENABLE_TESTING}>>:DOCTEST_CONFIG_DISABLE>
                               )
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON
                                               WINDOWS_EXPORT_ALL_SYMBOLS ON
                          )
endfunction()
