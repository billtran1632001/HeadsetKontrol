// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

import QtQuick

import org.kde.ki18n
import org.kde.kirigamiaddons.settings as KirigamiSettings

KirigamiSettings.ConfigurationView {
    modules: [
        KirigamiSettings.ConfigurationModule {
            icon.name: "configure"
            initialProperties: () => ({
                        title: Qt.binding(() => this.text)
                    })
            moduleId: "general"
            page: () => Qt.createComponent("vn.name.billtran.headsetkontrol", "HKGeneralConfigPage")
            text: KI18n.i18nc("@title:tab", "General")
        },
        KirigamiSettings.ConfigurationModule {
            icon.name: "interface"
            initialProperties: () => ({
                        title: Qt.binding(() => this.text)
                    })
            moduleId: "user-interface"
            page: () => Qt.createComponent("vn.name.billtran.headsetkontrol", "HKUserInterfaceConfigPage")
            text: KI18n.i18nc("@title:tab", "User Interface")
        },
        KirigamiSettings.ConfigurationModule {
            icon.name: "audio-headset"
            initialProperties: () => ({
                        title: Qt.binding(() => this.text)
                    })
            moduleId: "headsetcontrol"
            page: () => Qt.createComponent("vn.name.billtran.headsetkontrol", "HKHeadsetControlConfigPage")
            text: KI18n.i18nc("@title:tab", "Control")
        },
        KirigamiSettings.ShortcutsConfigurationModule {
            // qmllint disable missing-type unqualified
            application: hkKirigamiApp
            // qmllint enable  missing-type unqualified
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "about"
            text: KI18n.i18nc("@action:button", "About HeadsetKontrol")
            icon.name: "help-about"
            page: () => Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage")
            category: KI18n.i18nc("@title:group", "About")
        }
    ]
}
