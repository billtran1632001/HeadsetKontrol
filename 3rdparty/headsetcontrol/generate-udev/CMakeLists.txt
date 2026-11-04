# SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(headsetkontrol-generate-udev
    generate-udev.cpp
)

target_link_libraries(headsetkontrol-generate-udev PRIVATE
    HeadsetControl::HeadsetControl
)

set(HeadsetControl_UDEV_RULES "${CMAKE_BINARY_DIR}/80-headsetkontrol-headsets.rules")
set(HeadsetControl_UDEV_RULES "${HeadsetControl_UDEV_RULES}" PARENT_SCOPE)

add_custom_target(generate-udev
    ALL
    DEPENDS headsetkontrol-generate-udev
    COMMAND headsetkontrol-generate-udev > "${HeadsetControl_UDEV_RULES}"
    BYPRODUCTS "${HeadsetControl_UDEV_RULES}"
    COMMENT "Generating UDEV rules..."
    VERBATIM
)

find_package(PkgConfig QUIET)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(UDEV udev QUIET)
    pkg_get_variable(UDEV_RULES_DIR udev udevdir)

    if(UDEV_RULES_DIR)
        set(UDEV_RULES_INSTALL_DIR "${UDEV_RULES_DIR}/rules.d")
    endif()
endif()

if(NOT UDEV_RULES_INSTALL_DIR)
    set(UDEV_RULES_INSTALL_DIR "/lib/udev/rules.d")
endif()

set(UDEV_RULES_INSTALL_DIR "${UDEV_RULES_INSTALL_DIR}" CACHE PATH "Directory for udev rules")

install(FILES
        "${HeadsetControl_UDEV_RULES}"
    DESTINATION "${UDEV_RULES_INSTALL_DIR}"
)