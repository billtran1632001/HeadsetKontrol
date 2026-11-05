// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigami.platform as KirigamiPlatform

import org.kde.ki18n
import org.kde.kirigami.layouts as KirigamiLayouts
import org.kde.kirigamiaddons.actions as KirigamiActions

KirigamiActions.StatefulWindow {
    id: appWindow

    title: KI18n.i18nc("@title:window", "HeadsetKontrol")

    windowName: "HeadsetKontrol"

    minimumWidth: KirigamiPlatform.Units.gridUnit * 20
    minimumHeight: KirigamiPlatform.Units.gridUnit * 20

    application: hkKirigamiApp

    HKKirigamiApplication {
        id: hkKirigamiApp

        configurationView: HKConfigView {}
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: !KirigamiPlatform.Settings.isMobile

        actions: [
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.HCStart)
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.HCStop)
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.HCRefresh)
            },
            Kirigami.Action {
                separator: true
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.CommandBar)
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.Configure)
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.ConfigureKeybinding)
            },
            Kirigami.Action {
                separator: true
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.About)
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.BugReport)
            },
            Kirigami.Action {
                separator: true
            },
            Kirigami.Action {
                fromQAction: hkKirigamiApp.action(HKKirigamiApplication.Quit)
            }
        ]
    }

    pageStack {
        initialPage: HKDevicesListPage {
            onSelectedDeviceChanged: {
                if (selectedDevice) {
                    appWindow.pageStack.push(devicePageComponent.createObject(null, {
                        device: selectedDevice
                    }));
                } else {
                    appWindow.pageStack.pop(this);
                }
            }
        }
    }

    Component {
        id: devicePageComponent

        HKDeviceControlPage {
        }
    }
}
