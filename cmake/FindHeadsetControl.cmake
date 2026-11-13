# SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

# Find headers
find_path(HeadsetControl_INCLUDE_DIR
    NAMES
        headsetcontrol.hpp
        headsetcontrol_c.h
    PATH_SUFFIXES headsetcontrol
)

find_package(hidapi REQUIRED)

# Find static library
if(MSVC)
    set(_headsetcontrol_static_lib_name headsetcontrol_static.lib)
else()
    set(_headsetcontrol_static_lib_name libheadsetcontrol.a)
endif()

find_library(HeadsetControl_STATIC_LIBRARY
    NAMES
        "${_headsetcontrol_static_lib_name}"
        headsetcontrol_static
)

if(HeadsetControl_STATIC_LIBRARY AND NOT TARGET HeadsetControl::HeadsetControl-static)
    add_library(HeadsetControl::HeadsetControl-static STATIC IMPORTED)

    set_target_properties(HeadsetControl::HeadsetControl-static PROPERTIES
        IMPORTED_LOCATION "${HeadsetControl_STATIC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${HeadsetControl_INCLUDE_DIR}"
    )

    set(HeadsetControl-static_LINK_LIBS hidapi::hidapi)
    if(NOT MSVC)
        list(APPEND HeadsetControl-static_LINK_LIBS m)
    endif()

    set_property(TARGET HeadsetControl::HeadsetControl-static APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "${HeadsetControl-static_LINK_LIBS}"
    )

    set(HeadsetControl_HeadsetControl-static_FOUND TRUE)
endif()

# Find shared library
if(WIN32 OR MSVC)
    find_library(HeadsetControl_IMPORT_LIBRARY
        NAMES
            headsetcontrol
    )
    find_file(HeadsetControl_SHARED_LIBRARY
        NAMES
            headsetcontrol.dll
            libheadsetcontrol.dll
    )
else()
    find_library(HeadsetControl_SHARED_LIBRARY
        NAMES
            libheadsetcontrol.so
            libheadsetcontrol.dylib
            headsetcontrol
    )
endif()

if(HeadsetControl_SHARED_LIBRARY AND NOT TARGET HeadsetControl::HeadsetControl-shared)
    add_library(HeadsetControl::HeadsetControl-shared SHARED IMPORTED)

    set_target_properties(HeadsetControl::HeadsetControl-shared PROPERTIES
        IMPORTED_LOCATION "${HeadsetControl_SHARED_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${HeadsetControl_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "hidapi::include"
    )

    if((WIN32 OR MSVC) AND HeadsetControl_IMPORT_LIBRARY)
        set_target_properties(HeadsetControl::HeadsetControl-shared PROPERTIES
            IMPORTED_IMPLIB "${HeadsetControl_IMPORT_LIBRARY}"
        )
    endif()

    set(HeadsetControl_HeadsetControl-shared_FOUND TRUE)
endif()

# Find UDev rules file (Linux only)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_file(HeadsetControl_UDEV_RULES
        NAMES
            70-headsets.rules
        PATH_SUFFIXES
            lib/udev/rules.d
            usr/lib/udev/rules.d
            udev/rules.d
    )

    message("Found UDEV rules: ${HeadsetControl_UDEV_RULES}")
endif()

# Handling components
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HeadsetControl
    REQUIRED_VARS HeadsetControl_INCLUDE_DIR
    HANDLE_COMPONENTS
)

if(HeadsetControl_FOUND)
    set(HeadsetControl_INCLUDE_DIRS "${HeadsetControl_INCLUDE_DIR}")

    # Determine component selection logic:
    # - If ONLY one component is specified, use that specific component.
    # - If BOTH components are specified OR NO components are specified, default to static first, then shared.
    set(_want_static FALSE)
    set(_want_shared FALSE)

    if(HeadsetControl_FIND_COMPONENTS)
        if("HeadsetControl-static" IN_LIST HeadsetControl_FIND_COMPONENTS)
            set(_want_static TRUE)
        endif()
        if("HeadsetControl-shared" IN_LIST HeadsetControl_FIND_COMPONENTS)
            set(_want_shared TRUE)
        endif()
    else()
        set(_want_static TRUE)
        set(_want_shared TRUE)
    endif()

    set(_selected_target "")

    if(_want_static AND _want_shared)
        if(TARGET HeadsetControl::HeadsetControl-static)
            set(_selected_target HeadsetControl::HeadsetControl-static)
            set(HeadsetControl_LIBRARIES "${HeadsetControl_STATIC_LIBRARY}")
        elseif(TARGET HeadsetControl::HeadsetControl-shared)
            set(_selected_target HeadsetControl::HeadsetControl-shared)
            set(HeadsetControl_LIBRARIES "${HeadsetControl_SHARED_LIBRARY}")
        endif()
    elseif(_want_static)
        if(TARGET HeadsetControl::HeadsetControl-static)
            set(_selected_target HeadsetControl::HeadsetControl-static)
            set(HeadsetControl_LIBRARIES "${HeadsetControl_STATIC_LIBRARY}")
        endif()
    elseif(_want_shared)
        if(TARGET HeadsetControl::HeadsetControl-shared)
            set(_selected_target HeadsetControl::HeadsetControl-shared)
            set(HeadsetControl_LIBRARIES "${HeadsetControl_SHARED_LIBRARY}")
        endif()
    endif()

    if(_selected_target AND NOT TARGET HeadsetControl::HeadsetControl)
        add_library(HeadsetControl::HeadsetControl INTERFACE IMPORTED)
        target_link_libraries(HeadsetControl::HeadsetControl INTERFACE "${_selected_target}")

        if(HeadsetControl_UDEV_RULES)
            set_target_properties(HeadsetControl::HeadsetControl PROPERTIES
                UDEV_RULES "${HeadsetControl_UDEV_RULES}"
            )
            if(TARGET HeadsetControl::HeadsetControl-static)
                set_target_properties(HeadsetControl::HeadsetControl-static PROPERTIES
                    UDEV_RULES "${HeadsetControl_UDEV_RULES}"
                )
            endif()
            if(TARGET HeadsetControl::HeadsetControl-shared)
                set_target_properties(HeadsetControl::HeadsetControl-shared PROPERTIES
                    UDEV_RULES "${HeadsetControl_UDEV_RULES}"
                )
            endif()
        endif()
    endif()
endif()
