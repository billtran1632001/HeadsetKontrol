// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include "HKHeadsetControl.hpp" // IWYU pragma: export

#include <QMutex>
#include <QObjectComputedProperty>
#include <QThread>
#include <QTimer>

class HKHeadsetControlWorker : public QObject
{
    Q_OBJECT
public:
    HKHeadsetControlWorker(HKHeadsetControl *client);
    ~HKHeadsetControlWorker() override;

    [[nodiscard]] QList<HKDevice *> devices() const
    {
        QMutexLocker lock(&m_mutex);
        return m_devices;
    }

    // Always call these methods in HKHeadsetControlWorker's thread
    void start();
    void stop();
    void update();

Q_SIGNALS:
    void finished();
    void updating();
    void updated();

    void devicesAdded(const QList<HKDevice *> &);
    void devicesRemoved(const QList<HKDevice *> &);

private:
    mutable QMutex m_mutex;

    HKHeadsetControl *const m_client;

    bool m_started;
    bool m_discoverAll;

    QTimer m_updateTimer;

    QList<HKDevice *> m_devices;

    void forceUpdate();
    void doUpdate(
#ifdef HK_USE_SOLID
        bool forceRediscover
#endif // HK_USE_SOLID
    );
};
