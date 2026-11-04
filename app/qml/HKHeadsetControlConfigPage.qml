// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigami.platform as KirigamiPlatform
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Information")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: HKHeadsetControl.version
            description: KI18n.i18nc("@label:textbox", "HeadsetControl version")
        }

        FormCard.FormButtonDelegate {
            text: KI18n.i18ncp("@action:button", "Found %1 supported device", "Found %1 supported devices", HKHeadsetControl.supportedDevices.length)
            description: supportedDevicesOverlaySheet.title

            onClicked: supportedDevicesOverlaySheet.open()

            Kirigami.OverlaySheet {
                id: supportedDevicesOverlaySheet

                title: KI18n.i18nc("@title:window", "Supported devices")
                parent: root

                ListView {
                    model: HK.sortStringLocaleAware(HKHeadsetControl.supportedDevices)
                    implicitWidth: KirigamiPlatform.Units.gridUnit * 25
                    spacing: KirigamiPlatform.Units.smallSpacing

                    delegate: Kirigami.SelectableLabel {
                        required property int index
                        required property string modelData

                        leftPadding: KirigamiPlatform.Units.largeSpacing
                        rightPadding: leftPadding
                        topPadding: index === 0 ? leftPadding : 0
                        bottomPadding: index === ListView.view.count - 1 ? leftPadding : 0
                        text: modelData
                    }
                }
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Options")
    }

    FormCard.FormCard {
        FormCard.FormSpinBoxDelegate {
            id: updateDelayDelegate

            label: KI18n.i18nc("@label:spinbox", "Update delay")
            description: KI18n.i18nc("@info:usagetip update delay", "The delay between updates (in milliseconds).")
            statusMessage: value === 0 ? KI18n.i18nc("@info:status when update delay is 0", "Auto-update is disabled.") : ""
            from: 0
            to: 600000
            stepSize: 100
            value: HKConfig.updateDelay
            enabled: !HKConfig.isUpdateDelayImmutable

            onValueChanged: Qt.callLater(() => {
                HKConfig.updateDelay = value;
                HKConfig.save();
            })
        }

        FormCard.FormSpinBoxDelegate {
            id: timeoutDelegate

            label: KI18n.i18nc("@label:spinbox", "Timeout")
            description: KI18n.i18nc("@info:usagetip timeout", "The maximum time allowed for an update to run before it is stopped (in milliseconds)")
            statusMessage: value === 0 ? KI18n.i18nc("@info:status when timeout is 0", "Operations will not time out.") : ""
            from: 0
            stepSize: 100
            to: updateDelayDelegate.value
            value: HKConfig.timeout
            enabled: !HKConfig.isTimeoutImmutable

            onValueChanged: Qt.callLater(() => {
                HKConfig.timeout = value;
                HKConfig.save();
            })
        }

        FormCard.FormSwitchDelegate {
            id: discoverAllDelegate

            checked: HKConfig.discoverAll
            description: KI18n.i18nc("@info:usagetip", "Discover all USB devices with the same device ID.")
            text: KI18n.i18nc("@option:check", "Discover all devices")
            enabled: !HKConfig.isDiscoverAllImmutable

            onCheckedChanged: Qt.callLater(() => {
                HKConfig.discoverAll = checked;
                HKConfig.save();
            })
        }

        // FormCard.FormSwitchDelegate {
        //     id: stopOnRefreshErrorDelegate
        //
        //     checked: HKConfig.stopOnRefreshError
        //     description: KI18n.i18nc("@info:usagetip", "Stops automatic update when an error occurred during a refresh.")
        //     text: KI18n.i18nc("@option:check", "Stop on refresh error")
        //     enabled: !HKConfig.isStopOnRefreshErrorImmutable
        //
        //     onCheckedChanged: Qt.callLater(() => {
        //         HKConfig.stopOnRefreshError = checked;
        //         HKConfig.save();
        //     })
        // }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Test Device")
    }

    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            id: testDeviceEnabledDelegate

            checked: HKConfig.testDeviceEnabled
            description: KI18n.i18nc("@info:usagetip", "Enable reporting of the fake test device.")
            text: KI18n.i18nc("@option:check", "Report test device")
            enabled: !HKConfig.isTestDeviceEnabledImmutable

            onCheckedChanged: Qt.callLater(() => {
                HKConfig.testDeviceEnabled = checked;
                HKConfig.save();
            })
        }

        FormCard.FormSpinBoxDelegate {
            id: testProfileDelegate

            label: KI18n.i18nc("@option:check", "Test device profile")
            enabled: testDeviceEnabledDelegate.checked && testDeviceEnabledDelegate.enabled
            from: 0
            stepSize: 1
            to: 10
            value: HKConfig.testProfile

            onValueChanged: Qt.callLater(() => {
                HKConfig.testProfile = value;
                HKConfig.save();
            })
        }
    }
}
