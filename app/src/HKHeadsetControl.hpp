// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include <QObjectComputedProperty>
#include <QThread>
#include <QtQmlIntegration/qqmlintegration.h>

class HKDevice;

class HKHeadsetControlWorker;
class HKHeadsetControl : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString version READ version CONSTANT FINAL)
    Q_PROPERTY(QStringList supportedDevices READ supportedDevices CONSTANT FINAL)
    Q_PROPERTY(bool updating READ isUpdating BINDABLE bindableUpdating NOTIFY updatingChanged FINAL)
    Q_PROPERTY(bool running READ isRunning BINDABLE bindableRunning NOTIFY runningChanged FINAL)
    Q_PROPERTY(QList<HKDevice *> devices READ devices NOTIFY devicesChanged FINAL)

    friend class HKHeadsetControlStatic;
    HKHeadsetControl();

public:
    ~HKHeadsetControl() override;

    [[nodiscard]] static HKHeadsetControl *instance();
    [[nodiscard]] static HKHeadsetControl *create(class QQmlEngine *, class QJSEngine *);

    [[nodiscard]] static const QString &version() noexcept;

    [[nodiscard]] static const QStringList &supportedDevices() noexcept;

    [[nodiscard]] static bool isUpdating()
    {
        return static_cast<const HKHeadsetControl *>(instance())->m_updating;
    }

    [[nodiscard]] static QBindable<bool> bindableUpdating()
    {
        return &static_cast<const HKHeadsetControl *>(instance())->m_updating;
    }

    [[nodiscard]] static bool isRunning()
    {
        return static_cast<const HKHeadsetControl *>(instance())->m_running;
    }

    [[nodiscard]] static QBindable<bool> bindableRunning()
    {
        return &static_cast<const HKHeadsetControl *>(instance())->m_running;
    }

    [[nodiscard]] static QList<HKDevice *> devices();

public Q_SLOTS:
    static void start();
    static void stop();
    static void update();

Q_SIGNALS:
    void updatingChanged(bool);
    void runningChanged(bool);

    void devicesChanged(const QList<HKDevice *> &);

    void devicesAdded(const QList<HKDevice *> &);
    void deviceAdded(HKDevice *);

    void devicesRemoved(const QList<HKDevice *> &);
    void deviceRemoved(HKDevice *);

    void devicesUpdated(const QList<HKDevice *> &);
    void deviceUpdated(HKDevice *);

private:
    QThread m_workerThread;
    std::unique_ptr<HKHeadsetControlWorker> m_worker;

    Q_OBJECT_BINDABLE_PROPERTY(HKHeadsetControl, bool, m_updating, &HKHeadsetControl::updatingChanged)
    Q_OBJECT_BINDABLE_PROPERTY(HKHeadsetControl, bool, m_running, &HKHeadsetControl::runningChanged)
};

Q_MOC_INCLUDE("HKDevice.hpp")