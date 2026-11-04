// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.ki18n

import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegates
import org.kde.kirigami.platform as KirigamiPlatform

Kirigami.ScrollablePage {
    id: devicesPage

    title: KI18n.i18nc("@title", "Devices")

    readonly property Kirigami.AbstractApplicationWindow window: Kirigami.AbstractApplicationWindow.window as Kirigami.AbstractApplicationWindow
    readonly property HKKirigamiApplication application: window.application as HKKirigamiApplication

    property int sortOrder: Qt.AscendingOrder
    readonly property HKDevice selectedDevice: internal.selectedDevice

    actions: [
        Kirigami.Action {
            icon.name: "view-sort"
            text: KI18n.i18nc("@action:intoolbar", "Sort")

            Kirigami.Action {
                icon.name: "view-sort-ascending"
                text: KI18n.i18nc("@action:inmenu", "Ascending")
                autoExclusive: true
                checkable: true
                checked: true

                onToggled: proxyModel.sortOrder = Qt.AscendingOrder
            }

            Kirigami.Action {
                icon.name: "view-sort-descending"
                text: KI18n.i18nc("@action:inmenu", "Descending")
                autoExclusive: true
                checkable: true

                onToggled: proxyModel.sortOrder = Qt.DescendingOrder
            }
        }
    ]

    titleDelegate: Kirigami.SearchField {
        onTextChanged: proxyModel.filter = text

        Layout.fillWidth: true
        Layout.maximumWidth: Kirigami.Units.gridUnit * 15

        Component.onCompleted: Qt.callLater(() => {
            forceActiveFocus();
        })
    }

    ListView {
        id: listView

        spacing: 0
        focus: true

        model: HKDevicesProxyModel {
            id: proxyModel

            sourceModel: HKDevicesModel {}
        }

        delegate: DeviceDelegate {}

        onCountChanged: {
            if (count > 0) {
                if (!currentItem) {
                    currentIndex = 0;
                }
                if (!internal.selectedDevice) {
                    internal.selectedDevice = (currentItem as DeviceDelegate)?.device ?? null;
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent

            width: Math.min(listView.width - (Kirigami.Units.largeSpacing * 4), implicitWidth)
            height: Math.min(listView.height - (Kirigami.Units.largeSpacing * 4), implicitHeight)

            type: Kirigami.PlaceholderMessage.Informational

            visible: HKHeadsetControl.devices.length === 0 && listView.count === 0

            icon {
                name: "headsetkontrol"

                height: KirigamiPlatform.Units.iconSizes.huge
                width: height
            }

            text: KI18n.i18nc("@info", "No device detected")

            explanation: !HKHeadsetControl.running ? KI18n.i18nc("@info:usagetip headsetcontrol update stopped", "Update is currently stoppped. Start updating to receive information about your devices.") : ""

            helpfulAction: !HKHeadsetControl.running ? startAction : null

            Kirigami.Action {
                id: startAction
                fromQAction: devicesPage.application.action(HKKirigamiApplication.HCStart)
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent

            width: Math.min(listView.width - (Kirigami.Units.largeSpacing * 4), implicitWidth)
            height: Math.min(listView.height - (Kirigami.Units.largeSpacing * 4), implicitHeight)

            type: Kirigami.PlaceholderMessage.Informational

            visible: HKHeadsetControl.devices.length !== 0 && listView.count === 0

            icon {
                name: "error"

                height: KirigamiPlatform.Units.iconSizes.huge
                width: height
            }

            text: KI18n.i18nc("@info empty devices list", "Empty")

            explanation: KI18n.i18nc("@info:usagetip", "No device found based on your search")
        }
    }

    component DeviceDelegate: Controls.ItemDelegate {
        id: delegate

        required property int index
        required property hkDeviceId deviceId
        required property string deviceName
        required property string deviceVendorName
        required property string deviceProductName
        required property HKDevice device

        readonly property Controls.Action setPrimaryDeviceAction: Kirigami.Action {
            text: KI18n.i18nc("@action", "Set as primary device")
            icon.name: "favorite"

            onTriggered: {
                devicesPage.window.showPassiveNotification(KI18n.i18nc("@info", "Set device %1 as primary", delegate.deviceProductName));
            }
        }

        readonly property Controls.Action refreshDeviceAction: Kirigami.Action {
            text: KI18n.i18nc("@action", "Refresh")
            icon.name: "view-refresh"

            onTriggered: {
                devicesPage.window.showPassiveNotification(KI18n.i18nc("@info", "Refreshing device %1", delegate.deviceProductName));
            }
        }

        width: ListView.view.width
        focus: true
        activeFocusOnTab: true
        highlighted: ListView.isCurrentItem

        Controls.ContextMenu.menu: delegateContextMenu
        KirigamiPlatform.StyleHints.useAlternateBackgroundColor: true

        contentItem: RowLayout {
            spacing: KirigamiPlatform.Units.smallSpacing

            KirigamiDelegates.TitleSubtitle {
                Layout.fillWidth: true
                Layout.maximumWidth: Math.ceil(implicitWidth)
                Layout.alignment: Qt.AlignLeft

                title: delegate.deviceProductName
                subtitle: `${delegate.deviceName === delegate.deviceProductName ? '' : `${delegate.deviceName} | `}${delegate.deviceVendorName} | ${delegate.deviceId}`

                elide: Text.ElideRight
            }

            Kirigami.ActionToolBar {
                Layout.fillWidth: true
                Layout.fillHeight: true

                actions: [delegate.setPrimaryDeviceAction, delegate.refreshDeviceAction]
                alignment: Qt.AlignRight
                display: Controls.AbstractButton.IconOnly
                flat: true
            }
        }

        onClicked: {
            ListView.view.currentIndex = index;
            internal.selectedDevice = device;
        }
    }

    Controls.Menu {
        id: delegateContextMenu

        property Kirigami.Action setPrimaryDeviceAction: (parent as DeviceDelegate)?.setPrimaryDeviceAction ?? null
        property Kirigami.Action refreshDeviceAction: (parent as DeviceDelegate)?.refreshDeviceAction ?? null

        closePolicy: Controls.Popup.CloseOnPressOutside | Controls.Popup.CloseOnEscape

        Controls.MenuItem {
            action: delegateContextMenu.setPrimaryDeviceAction
        }

        Controls.MenuItem {
            action: delegateContextMenu.refreshDeviceAction
        }
    }

    QtObject {
        id: internal

        property HKDevice selectedDevice
    }
}
