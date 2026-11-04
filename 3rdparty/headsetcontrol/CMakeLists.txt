# SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

block()
    set(BUILD_SHARED_LIBRARY OFF)
    set(HEADSETCONTROL_VERSION "4.2.0")

    add_subdirectory(HeadsetControl SYSTEM)

    set_package_properties(hidapi
        PROPERTIES
            TYPE REQUIRED
            PURPOSE "HeadsetControl dependency."
    )

    set_property(TARGET headsetcontrol_lib PROPERTY SYSTEM ON)
    if(MSVC)
        target_compile_options(headsetcontrol_lib PRIVATE /W0)
    else()
        target_compile_options(headsetcontrol_lib PRIVATE -w)
    endif()

    add_library(HeadsetControl::HeadsetControl-static ALIAS headsetcontrol_lib)
    add_library(HeadsetControl::HeadsetControl ALIAS headsetcontrol_lib)
endblock()

add_subdirectory(generate-udev)
set(HeadsetControl_UDEV_RULES "${HeadsetControl_UDEV_RULES}" PARENT_SCOPE)

