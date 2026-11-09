# SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.28)

project(HeadsetKontrol
    VERSION 3.0.0
    DESCRIPTION "Control your headsets with HeadsetControl."
    HOMEPAGE_URL "https://gitlab.com/billtran1632001/headsetkontrol"
    LANGUAGES CXX
)

include(FeatureSummary)

option(USE_SOLID "Use KDE Solid to detect when a device is added or removed. This help prevents constant polling of HIDAPI." ON)

option(BUILD_FLATPAK "Build for Flatpak" OFF)
option(BUILD_APPIMAGE "Build for AppImage" OFF)

if(BUILD_FLATPAK OR BUILD_APPIMAGE)
    set(HK_CONTAINED ON)
endif()

option(USE_VENDORED_HEADSETCONTROL "Use vendored HeadsetControl library, forcing static linking" ON)

if(USE_VENDORED_HEADSETCONTROL)
    set(USE_SHARED_HEADSETCONTROL OFF)
else()
    option(USE_SHARED_HEADSETCONTROL "Use shared HeadsetControl library" OFF)
endif()

if(HK_CONTAINED OR NOT LINUX OR ANDROID)
    set(INSTALL_UDEV_RULES OFF)
else()
    option(INSTALL_UDEV_RULES "Install the UDEV rules for HeadsetControl" ON)
endif()

set(QT6_MIN_VERSION 6.8.0)
set(KF6_MIN_VERSION 6.24.0)
set(KIRIGAMI_ADDONS_MIN_VERSION 1.15.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM "${KF6_MIN_VERSION}" REQUIRED NO_MODULE)
list(PREPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(KDEInstallDirs6)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(ECMDeprecationSettings)
include(ECMFindQmlModule)
include(ECMInstallIcons)
include(ECMQmlModule)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)

find_package(Qt6 "${QT6_MIN_VERSION}" REQUIRED COMPONENTS
    Qml
    Quick
    Widgets
)

find_package(KF6 "${KF6_MIN_VERSION}" REQUIRED COMPONENTS
    Config
    CoreAddons
    Crash
    I18n
    Kirigami
    NotifyConfig # TODO: Evaluate removing
)

if(USE_SOLID)
    find_package(KF6Solid "${KF6_MIN_VERSION}" REQUIRED COMPONENTS)
    set_package_properties(KF6Solid
        PROPERTIES
            TYPE REQUIRED
            PURPOSE "Detect when a device is added or removed. This help prevents constant polling of HIDAPI."
    )
endif()

find_package(KF6KirigamiAddons ${KIRIGAMI_ADDONS_MIN_VERSION} REQUIRED)
set_package_properties(KF6KirigamiAddons
    PROPERTIES
        TYPE REQUIRED
        PURPOSE "Required application components and runtime dependency."
)

if(NOT USE_VENDORED_HEADSETCONTROL)
    if(USE_SHARED_HEADSETCONTROL)
        find_package(HeadsetControl REQUIRED COMPONENTS HeadsetControl-shared)
    else()
        find_package(HeadsetControl REQUIRED COMPONENTS HeadsetControl-static)
    endif()
endif()

ecm_set_disabled_deprecation_versions(
    QT "${QT6_MIN_VERSION}"
    KF "${KF6_MIN_VERSION}"
    KIRIGAMIAPP "${KIRIGAMI_ADDONS_MIN_VERSION}"
    KIRIGAMIADDONSSTATEFULAPP "${KIRIGAMI_ADDONS_MIN_VERSION}"
)

ecm_find_qmlmodule(org.kde.ki18n "${KF6_MIN_VERSION}" REQUIRED)
ecm_find_qmlmodule(org.kde.kirigami "${KF6_MIN_VERSION}" REQUIRED)
ecm_find_qmlmodule(org.kde.kirigami.delegates "${KF6_MIN_VERSION}" REQUIRED)
ecm_find_qmlmodule(org.kde.kirigami.platform "${KF6_MIN_VERSION}" REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.actions "${KIRIGAMI_ADDONS_MIN_VERSION}" REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.formcard "${KIRIGAMI_ADDONS_MIN_VERSION}" REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.settings "${KIRIGAMI_ADDONS_MIN_VERSION}" REQUIRED)

add_subdirectory(3rdparty)

add_subdirectory(app)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

install(FILES vn.name.billtran.headsetkontrol.desktop       DESTINATION "${KDE_INSTALL_APPDIR}")
install(FILES vn.name.billtran.headsetkontrol.metainfo.xml  DESTINATION "${KDE_INSTALL_METAINFODIR}")

ecm_install_icons(ICONS
        icons/sc-apps-headsetkontrol.svg
        icons/sc-status-headsetkontrol-battery-000-charging.svg
        icons/sc-status-headsetkontrol-battery-000.svg
        icons/sc-status-headsetkontrol-battery-010-charging.svg
        icons/sc-status-headsetkontrol-battery-010.svg
        icons/sc-status-headsetkontrol-battery-020-charging.svg
        icons/sc-status-headsetkontrol-battery-020.svg
        icons/sc-status-headsetkontrol-battery-030-charging.svg
        icons/sc-status-headsetkontrol-battery-030.svg
        icons/sc-status-headsetkontrol-battery-040-charging.svg
        icons/sc-status-headsetkontrol-battery-040.svg
        icons/sc-status-headsetkontrol-battery-050-charging.svg
        icons/sc-status-headsetkontrol-battery-050.svg
        icons/sc-status-headsetkontrol-battery-060-charging.svg
        icons/sc-status-headsetkontrol-battery-060.svg
        icons/sc-status-headsetkontrol-battery-070-charging.svg
        icons/sc-status-headsetkontrol-battery-070.svg
        icons/sc-status-headsetkontrol-battery-080-charging.svg
        icons/sc-status-headsetkontrol-battery-080.svg
        icons/sc-status-headsetkontrol-battery-090-charging.svg
        icons/sc-status-headsetkontrol-battery-090.svg
        icons/sc-status-headsetkontrol-battery-100-charging.svg
        icons/sc-status-headsetkontrol-battery-100.svg
    DESTINATION "${KDE_INSTALL_ICONDIR}"
)

ecm_qt_install_logging_categories(
    EXPORT HK_LOGGING
    FILE vn.name.billtran.headsetkontrol.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES
    3rdparty/HeadsetControl/generate-udev/*.cpp
    3rdparty/HeadsetControl/generate-udev/*.hpp
    app/*.cpp
    app/*.hpp
)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
