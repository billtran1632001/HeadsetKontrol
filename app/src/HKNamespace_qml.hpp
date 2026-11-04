// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include "HKDeviceId.hpp"
#include "HKNamespace.hpp"

#include <QtQmlIntegration/qqmlintegration.h>

class HKQml : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(HK)
    QML_SINGLETON
    QML_EXTENDED_NAMESPACE(HK)

    Q_PROPERTY(HKDeviceId emptyDeviceId READ emptyDeviceId CONSTANT FINAL)

public:
    Q_INVOKABLE [[nodiscard]] inline QStringList sortStringLocaleAware(QStringList list)
    {
        return HK::sortStringLocaleAware(list);
    }

    [[nodiscard]] inline static HKDeviceId emptyDeviceId() noexcept
    {
        return {};
    }
};