// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include "HKDeviceId.hpp"

#include <QMetaObject>
#include <QtQmlIntegration/qqmlintegration.h>

class HKDeviceInfo
{
    Q_GADGET
    QML_VALUE_TYPE(hkDeviceInfo)

    Q_PROPERTY(HKDeviceId id MEMBER id FINAL)
    Q_PROPERTY(QString name MEMBER name FINAL)
    Q_PROPERTY(QString vendorName MEMBER vendorName FINAL)
    Q_PROPERTY(QString productName MEMBER productName FINAL)
public:
    HKDeviceInfo() = default;

    HKDeviceInfo(HKDeviceId id, QString name, QString vendorName, QString productName);
    HKDeviceInfo(const class HKDevice *const device);

    HKDeviceId id;
    QString name;
    QString vendorName;
    QString productName;
};

Q_DECLARE_METATYPE(HKDeviceInfo)
Q_DECLARE_TYPEINFO(HKDeviceInfo, Q_RELOCATABLE_TYPE);