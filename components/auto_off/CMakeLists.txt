spl_add_source(src/auto_off.c)
spl_add_test_source(test/test_auto_off.cc)
spl_add_required_interface(components/rte)
spl_create_component(LONG_NAME "Auto Off")

if(BUILD_TESTING)
    target_compile_options(components_auto_off PRIVATE
        -include ${PROJECT_SOURCE_DIR}/components/platform_types/src/platform_types.h)
    target_compile_options(components_auto_off_test PRIVATE
        -include ${PROJECT_SOURCE_DIR}/components/platform_types/src/platform_types.h)
endif()
