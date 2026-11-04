# SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

if(INSTALL_UDEV_RULES AND NOT UDEV_RULES_INSTALL_DIR)
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

    set(UDEV_RULES_INSTALL_DIR "${UDEV_RULES_INSTALL_DIR}" CACHE PATH "Install location for UDEV rules")
endif()

add_subdirectory(headsetcontrol)

if(LINUX AND HeadsetControl_UDEV_RULES)
    set(HeadsetControl_UDEV_RULES "${HeadsetControl_UDEV_RULES}" PARENT_SCOPE)
endif()