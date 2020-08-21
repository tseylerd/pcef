# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

set(PCEF_SRCS
        library.h
        library.cpp
        PCefClient.cpp
        PCefClient.h
        PCefApp.h
        PCefApp.cpp
        mac_util.h
        mac_util.mm
        util.h
        util.cpp
        PCefRenderHandler.cpp
        PCefRenderHandler.h)

set(PCEF_HELPER_SRCS
        app_subprocess_impl.cpp
        process_helper_mac.cpp
        process_helper_mac.h
        )

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/build/install/)
APPEND_PLATFORM_SOURCES(PCEF_SRCS)
set(PCEF_LIBRARY_TARGET pcef)

APPEND_PLATFORM_SOURCES(PCEF_HELPER_SRCS)


#
# Shared configuration.
#

# Target binary names.
if(OS_MACOSX)
    set(PCEF_HELPER_TARGET "pcef_Helper")
    set(PCEF_TEST_HELPER_TARGET "pcef_test_Helper")
    set(PCEF_HELPER_OUTPUT_NAME "pcef Helper")
    set(PCEF_TEST_HELPER_OUTPUT_NAME "pcef_test Helper")
endif()

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()

#
# Mac OS X configuration.
#

if(OS_MACOSX)
    # Avoid CMP0042 policy errors.
    set(CMAKE_MACOSX_RPATH 1)

    # Avoid CMP0068 policy errors.
    if(POLICY CMP0068)
        cmake_policy(SET CMP0068 NEW)
    endif()

    if(USE_SANDBOX)
        # Logical target used to link the cef_sandbox library.
        ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    endif()

    # PCEF library target.
    add_library(${PCEF_LIBRARY_TARGET}
            ${PCEF_RESOURCES_SRCS}
            ${PCEF_SRCS}
            )

    install(TARGETS ${PCEF_LIBRARY_TARGET}
            DESTINATION ${CMAKE_INSTALL_PREFIX}
            )
    SET_LIBRARY_TARGET_PROPERTIES(${PCEF_LIBRARY_TARGET})
    add_dependencies(${PCEF_LIBRARY_TARGET} libcef_dll_wrapper)
    target_link_libraries(${PCEF_LIBRARY_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS})

    add_custom_command(
            TARGET ${PCEF_LIBRARY_TARGET}
            POST_BUILD
            # Build the main app bundle.
            # Copy the CEF framework into the main app bundle.
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_TARGET_OUT_DIR}/Chromium Embedded Framework.framework"
    )
    install(TARGETS libcef_dll_wrapper
            DESTINATION ${CMAKE_INSTALL_PREFIX}
            )
    # Create the multiple Helper app bundle targets.
    foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
        # Convert to a list and extract the suffix values.
        string(REPLACE ":" ";" _suffix_list ${_suffix_list})
        list(GET _suffix_list 0 _name_suffix)
        list(GET _suffix_list 1 _target_suffix)
        list(GET _suffix_list 2 _plist_suffix)

        # Define Helper target and output names.
        set(_helper_target "${PCEF_HELPER_TARGET}${_target_suffix}")
        set(_helper_output_name "${PCEF_HELPER_OUTPUT_NAME}${_name_suffix}")

        # Create Helper-specific variants of the helper-Info.plist file. Do this
        # manually because the configure_file command (which is executed as part of
        # MACOSX_BUNDLE_INFO_PLIST) uses global env variables and would insert the
        # wrong values with multiple targets.
        set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/pcef-Info${_target_suffix}.plist")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/resources/pcef-Info.plist" _plist_contents)
        string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
        string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
        string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
        file(WRITE ${_helper_info_plist} ${_plist_contents})

        # Create Helper executable target.
        add_executable(${_helper_target} MACOSX_BUNDLE ${PCEF_HELPER_SRCS})
        install(TARGETS ${_helper_target}
                DESTINATION ${CMAKE_INSTALL_PREFIX}
                )

        SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
        add_dependencies(${_helper_target} libcef_dll_wrapper)
        target_link_libraries(${_helper_target} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
        set_target_properties(${_helper_target} PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
                OUTPUT_NAME ${_helper_output_name}
                )

        if(USE_SANDBOX)
            target_link_libraries(${_helper_target} cef_sandbox_lib)
        endif()

        # Add the Helper as a dependency of the main executable target.
        add_dependencies(${PCEF_LIBRARY_TARGET} "${_helper_target}")
    endforeach()
endif()
