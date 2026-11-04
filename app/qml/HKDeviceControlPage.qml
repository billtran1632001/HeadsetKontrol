// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

import QtQuick

import org.kde.kirigamiaddons.formcard as KirigamiFormCard

KirigamiFormCard.FormCardPage {
    id: page

    required property HKDevice device

    QtObject {
        id: proxy

        readonly property string name: page.device?.name ?? ''
        readonly property string vendorName: page.device?.vendorName ?? ''
        readonly property string productName: page.device?.productName ?? ''
        readonly property hkDeviceId deviceId: page.device?.id ?? HK.emptyDeviceId
    }

    title: proxy.productName
}
