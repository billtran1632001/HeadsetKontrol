# SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(headsetkontrol)

ecm_add_qml_module(headsetkontrol
    URI vn.name.billtran.headsetkontrol
    VERSION "${PROJECT_VERSION}"
    DEPENDENCIES
        QtQuick
        QtQuick.Controls
        QtQuick.Layouts
        org.kde.ki18n
        org.kde.kirigami
        org.kde.kirigami.delegates
        org.kde.kirigami.platform
        org.kde.kirigamiaddons.actions
        org.kde.kirigamiaddons.formcard
        org.kde.kirigamiaddons.settings
)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX HK
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/HKVersion.h"
)

kconfig_add_kcfg_files(headsetkontrol GENERATE_MOC USE_RELATIVE_PATH src/HKConfig.kcfgc)
kconfig_add_kcfg_files(headsetkontrol GENERATE_MOC USE_RELATIVE_PATH src/HKDeviceConfig.kcfgc)

target_sources(headsetkontrol PRIVATE
    src/main.cpp

    src/HKConfigUpdate.cpp
    src/HKConfigUpdate.hpp
    src/HKDevice.cpp
    src/HKDevice.hpp
    src/HKDeviceId.cpp
    src/HKDeviceId.hpp
    src/HKDeviceInfo.cpp
    src/HKDeviceInfo.hpp
    src/HKDevice_p.hpp
    src/HKDevicesModel.cpp
    src/HKDevicesModel.hpp
    src/HKHeadsetControl.cpp
    src/HKHeadsetControl.hpp
    src/HKHeadsetControl_p.hpp
    src/HKKirigamiApplication.cpp
    src/HKKirigamiApplication.hpp
    src/HKLogging.cpp
    src/HKLogging.hpp
    src/HKNamespace.cpp
    src/HKNamespace.hpp
    src/HKNamespace_qml.hpp
)

ecm_target_qml_sources(headsetkontrol SOURCES
    qml/HKAppWindow.qml

    qml/HKConfigView.qml
    qml/HKGeneralConfigPage.qml
    qml/HKHeadsetControlConfigPage.qml
    qml/HKUserInterfaceConfigPage.qml

    qml/HKDevicesListPage.qml
    qml/HKDeviceControlPage.qml
)

add_dependencies(headsetkontrol generate-udev)
get_filename_component(_udev_rules_name "${HeadsetControl_UDEV_RULES}" NAME)
set_source_files_properties("${HeadsetControl_UDEV_RULES}" PROPERTIES
    QT_RESOURCE_ALIAS "${_udev_rules_name}"
)

qt_add_resources(headsetkontrol resources
    PREFIX /headsetkontrol
    FILES
        "${HeadsetControl_UDEV_RULES}"
)

target_include_directories(headsetkontrol PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

target_compile_definitions(headsetkontrol PRIVATE
    $<$<BOOL:"${USE_SOLID}">:HK_USE_SOLID>
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:QT_NO_DEBUG_OUTPUT>
)

target_link_libraries(headsetkontrol PRIVATE
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets

    KF6::ConfigGui
    KF6::CoreAddons
    KF6::Crash
    KF6::I18n
    KF6::I18nQml
    KF6::NotifyConfig
    $<$<BOOL:"${USE_SOLID}">:KF6::Solid>

    KirigamiApp
    KirigamiAddonsStatefulApp

    HeadsetControl::HeadsetControl
)

install(TARGETS headsetkontrol ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_qt_export_logging_category(
    IDENTIFIER HK_HC_LOGGING
    CATEGORY_NAME vn.name.billtran.headsetkontrol.headsetcontrol
    DESCRIPTION "HeadsetKontrol's HeadsetControl backend"
    EXPORT HK_LOGGING
)

ecm_qt_export_logging_category(
    IDENTIFIER HK_LOGGING
    CATEGORY_NAME vn.name.billtran.headsetkontrol
    DESCRIPTION "HeadsetKontrol"
    EXPORT HK_LOGGING
)
