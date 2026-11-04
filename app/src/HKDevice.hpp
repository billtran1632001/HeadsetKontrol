// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#pragma once

#include "HKNamespace.hpp"

#include <QObjectBindableProperty>
#include <QtQmlIntegration/qqmlintegration.h>

class HKDeviceId;
class HKHeadsetControl;

class HKDeviceWorker;
class HKDevice : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Managed by HKHeadsetControl")

    Q_PROPERTY(HKDeviceId id READ id CONSTANT FINAL)
    Q_PROPERTY(QString name READ name CONSTANT FINAL)
    Q_PROPERTY(QString vendorName READ vendorName CONSTANT FINAL)
    Q_PROPERTY(QString productName READ productName CONSTANT FINAL)
    Q_PROPERTY(HK::DeviceCapabilities capabilities READ capabilities NOTIFY capabilitiesChanged BINDABLE bindableCapabilities FINAL)

    friend class HKHeadsetControlWorker;
    friend class HKDeviceWorker;
    friend class HKDeviceInfo;

    HKDevice(struct HKDeviceInit &&init, HKHeadsetControl *parent);

public:
    ~HKDevice() override;

    [[nodiscard]] HKDeviceId id() const;

    [[nodiscard]] QString name() const;

    [[nodiscard]] QString vendorName() const;

    [[nodiscard]] QString productName() const;

    [[nodiscard]] HK::DeviceCapabilities capabilities() const
    {
        return m_capabilities;
    }

    [[nodiscard]] QBindable<HK::DeviceCapabilities> bindableCapabilities() const
    {
        return &m_capabilities;
    }

public Q_SLOTS:
    void update();

Q_SIGNALS:
    void capabilitiesChanged(HK::DeviceCapabilities);

private:
    HKHeadsetControl *const m_parent;
    std::unique_ptr<HKDeviceWorker> m_worker;

    Q_OBJECT_BINDABLE_PROPERTY(HKDevice, HK::DeviceCapabilities, m_capabilities, &HKDevice::capabilitiesChanged)
};

Q_MOC_INCLUDE("HKDeviceId.hpp")