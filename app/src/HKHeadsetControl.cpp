// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "HKHeadsetControl.hpp"
#include "HKHeadsetControl_p.hpp"

#include "HKConfig.hpp"
#include "HKDeviceId.hpp"
#include "HKDevice_p.hpp"

#define HK_LOGGING_CATEGORY HK_HC_LOGGING
#include "HKLogging.hpp"

#include "headsetcontrol.hpp"

#include <QApplicationStatic>
#include <QDebug>
#include <QQmlEngine>
#include <QThreadPool>

#ifdef HK_USE_SOLID
#include <Solid/DeviceNotifier>
#endif // HK_USE_SOLID

using namespace Qt::StringLiterals;

HK_APP_STATIC(HKHeadsetControl, g_helper)

HKHeadsetControlWorker::HKHeadsetControlWorker(HKHeadsetControl *client)
    : m_client{client}
    , m_started{false}
    , m_discoverAll{HKConfig::discoverAll()}
    , m_updateTimer{this}
{
    connect(
        HKConfig::self(),
        &HKConfig::testDeviceEnabledChanged,
        this,
        [this] {
            QMetaObject::invokeMethod(
                this,
                [this](bool enabled) {
                    if (headsetcontrol::isTestDeviceEnabled() == enabled) {
                        return;
                    }
                    headsetcontrol::enableTestDevice(enabled);
                    forceUpdate();
                },
                HKConfig::testDeviceEnabled());
        },
        Qt::DirectConnection);

    connect(
        HKConfig::self(),
        &HKConfig::timeoutChanged,
        this,
        [this] {
            QMetaObject::invokeMethod(
                this,
                [](uint timeout) {
                    if (std::cmp_equal(timeout, headsetcontrol::getDeviceTimeout())) {
                        return;
                    }
                    headsetcontrol::setDeviceTimeout(timeout);
                },
                HKConfig::timeout());
        },
        Qt::DirectConnection);

    connect(
        HKConfig::self(),
        &HKConfig::testProfileChanged,
        this,
        [this] {
            QMetaObject::invokeMethod(
                this,
                [this](uint profile) {
                    if (std::cmp_equal(profile, headsetcontrol::getTestProfile())) {
                        return;
                    }
                    headsetcontrol::setTestProfile(profile);
                    forceUpdate();
                },
                HKConfig::testProfile());
        },
        Qt::DirectConnection);

    connect(
        HKConfig::self(),
        &HKConfig::discoverAllChanged,
        this,
        [this] {
            QMetaObject::invokeMethod(
                this,
                [this](bool discoverAll) {
                    m_discoverAll = discoverAll;
                    forceUpdate();
                },
                HKConfig::discoverAll());
        },
        Qt::DirectConnection);

    connect(
        HKConfig::self(),
        &HKConfig::updateDelayChanged,
        this,
        [this] {
            QMetaObject::invokeMethod(&m_updateTimer,
                                      qOverload<std::chrono::milliseconds>(&QTimer::setInterval),
                                      std::chrono::milliseconds(HKConfig::updateDelay()));
        },
        Qt::DirectConnection);

    connect(&m_updateTimer, &QTimer::timeout, this, [this] {
        doUpdate(
#ifdef HK_USE_SOLID
            false
#endif // HK_USE_SOLID
        );
    });

    headsetcontrol::enableTestDevice(HKConfig::testDeviceEnabled());
    headsetcontrol::setDeviceTimeout(HKConfig::timeout());
    headsetcontrol::setTestProfile(HKConfig::testProfile());

#ifdef HK_USE_SOLID
    // There isn't a robust cross-platform way to get USB vendorID and productID from Solid.
    // Any device added/removed will trigger this.
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this, [this](const QString &uid) {
        HK_DEBUG("Solid device added: %s", qUtf8Printable(uid));
        forceUpdate();
    });
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this, [this](const QString &uid) {
        HK_DEBUG("Solid device removed: %s", qUtf8Printable(uid));
        forceUpdate();
    });
#endif // HK_USE_SOLID

    m_updateTimer.setTimerType(Qt::PreciseTimer);
    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(std::chrono::milliseconds{HKConfig::updateDelay()});
}

HKHeadsetControlWorker::~HKHeadsetControlWorker()
{
    qDeleteAll(m_devices);
}

void HKHeadsetControlWorker::start()
{
    if (m_started) {
        return;
    }
    m_started = true;

    doUpdate(
#ifdef HK_USE_SOLID
        true
#endif // HK_USE_SOLID
    );
}

void HKHeadsetControlWorker::stop()
{
    m_started = false;
    m_updateTimer.stop();
    Q_EMIT finished();
}

void HKHeadsetControlWorker::update()
{
    doUpdate(
#ifdef HK_USE_SOLID
        true
#endif // HK_USE_SOLID
    );
}

void HKHeadsetControlWorker::forceUpdate()
{
    if (!m_started) {
        return;
    }
    doUpdate(
#ifdef HK_USE_SOLID
        true
#endif // HK_USE_SOLID
    );
}

void HKHeadsetControlWorker::doUpdate(
#ifdef HK_USE_SOLID
    bool forceRediscover
#endif // HK_USE_SOLID
)
{
    Q_EMIT updating();

    QMutexLocker lock(&m_mutex);
    auto devices = m_devices;
    lock.unlock();

#ifdef HK_USE_SOLID
    if (forceRediscover) {
#endif // HK_USE_SOLID
        std::unordered_map<HKDeviceId, std::vector<headsetcontrol::Headset>> newDevicesMap;
        {
            auto hcDevices = m_discoverAll ? headsetcontrol::discoverAll() : headsetcontrol::discover();
            for (auto &hcDevice : hcDevices) {
                newDevicesMap[HKDeviceId{hcDevice.vendorId(), hcDevice.productId()}].push_back(std::move(hcDevice));
            }
        }

        std::unordered_map<HKDevice *, std::vector<headsetcontrol::Headset>> updateDevicesMap;

        for (auto newDevIt = newDevicesMap.begin(); newDevIt != newDevicesMap.end();) {
            {
                const auto name = newDevIt->second.front().name();
                HK_DEBUG().noquote().nospace() << "Device detected from HeadsetControl: " << name << " [" << newDevIt->first.toString() << "]";
            }

            auto oldDevIt = std::ranges::find_if(devices, [&newDevIt](const HKDevice *const device) {
                return device->id() == newDevIt->first;
            });

            if (oldDevIt != devices.end()) {
                updateDevicesMap[*oldDevIt] = std::move(newDevIt->second);
                devices.erase(oldDevIt);

                newDevIt = newDevicesMap.erase(newDevIt);
            } else {
                ++newDevIt;
            }
        }

        // Becareful with the following code order of operation.
        QList<HKDevice *> devicesToRemoved = std::move(devices);
        QList<HKDevice *> devicesToAdd;

        devices.clear();

        // Update existing
        {
            QList<HKDevice *> updatedDevices;

            for (auto &[device, hcHeadsets] : updateDevicesMap) {
                device->m_worker->m_hcHeadsets = std::move(hcHeadsets);
                if (device->m_worker->doUpdate()) {
                    updatedDevices.append(device);
                    Q_EMIT m_client->deviceUpdated(device);
                }
                devices.append(device);
            }

            if (!updatedDevices.isEmpty()) {
                Q_EMIT m_client->devicesUpdated(updatedDevices);
            }

            updateDevicesMap.clear();
        }

        // Create new
        {
            for (auto &pair : newDevicesMap) {
                auto *device = new HKDevice({std::move(pair.second), this}, m_client);
                HK_INFO("Device added: %s [%s]", qUtf8Printable(device->productName()), qUtf8Printable(device->id().toString()));
                device->m_worker->doUpdate();
                devicesToAdd << device;
            }

            devices.append(devicesToAdd);

            newDevicesMap.clear();
        }

        lock.relock();
        if (m_devices != devices) {
            m_devices = devices;
            lock.unlock();
            Q_EMIT m_client->devicesChanged(devices);
        }

        if (!devicesToAdd.isEmpty()) {
            for (auto *device : std::as_const(devicesToAdd)) {
                Q_EMIT m_client->deviceAdded(device);
            }
            Q_EMIT m_client->devicesAdded(devicesToAdd);
        }

        if (!devicesToRemoved.isEmpty()) {
            for (auto *device : std::as_const(devicesToRemoved)) {
                HK_INFO("Device removed: %s [%s]", qUtf8Printable(device->productName()), qUtf8Printable(device->id().toString()));
                Q_EMIT m_client->deviceRemoved(device);
                device->deleteLater();
            }
            Q_EMIT m_client->devicesRemoved(devicesToRemoved);
        }
#ifdef HK_USE_SOLID
    } else {
        QList<HKDevice *> updatedDevices;
        for (auto *device : std::as_const(devices)) {
            if (device->m_worker->doUpdate()) {
                updatedDevices.append(device);
                Q_EMIT m_client->deviceUpdated(device);
            }
        }

        if (!updatedDevices.isEmpty()) {
            Q_EMIT m_client->devicesUpdated(updatedDevices);
        }
    }
#endif // HK_USE_SOLID

    Q_EMIT updated();

    if (m_started) {
        m_updateTimer.start();
    } else {
        Q_EMIT finished();
    }
}

HKHeadsetControl::HKHeadsetControl()
    : m_worker{std::make_unique<HKHeadsetControlWorker>(this)}
{
    m_workerThread.setObjectName("HeadsetKontrolWorkerThread");

    connect(m_worker.get(), &HKHeadsetControlWorker::finished, this, [this] {
        m_running = false;
    });

    connect(m_worker.get(), &HKHeadsetControlWorker::updating, this, [this] {
        m_updating = true;
    });

    connect(m_worker.get(), &HKHeadsetControlWorker::updated, this, [this] {
        m_updating = false;
    });

    connect(m_worker.get(), &HKHeadsetControlWorker::devicesAdded, this, [this](const QList<HKDevice *> &addedDevices) {
        Q_EMIT devicesAdded(addedDevices);
        for (auto *const device : addedDevices) {
            Q_EMIT deviceAdded(device);
        }
    });

    connect(m_worker.get(), &HKHeadsetControlWorker::devicesRemoved, this, [this](const QList<HKDevice *> &removedDevices) {
        Q_EMIT devicesRemoved(removedDevices);
        for (auto *const device : removedDevices) {
            Q_EMIT deviceRemoved(device);
        }
    });

    connect(qApp, &QCoreApplication::aboutToQuit, m_worker.get(), &HKHeadsetControlWorker::stop);

    m_worker->moveToThread(&m_workerThread);

    m_workerThread.start();
}

HKHeadsetControl::~HKHeadsetControl()
{
    stop();

    m_workerThread.quit();
    m_workerThread.wait();
}

HKHeadsetControl *HKHeadsetControl::instance()
{
    return g_helper->instance;
}

HKHeadsetControl *HKHeadsetControl::create(QQmlEngine *, QJSEngine *)
{
    auto *const instance = HKHeadsetControl::instance();
    QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
    return instance;
}

const QString &HKHeadsetControl::version() noexcept
{
    static const auto version = QAnyStringView(headsetcontrol::version()).toString();
    return version;
}

const QStringList &HKHeadsetControl::supportedDevices() noexcept
{
    static const auto supportedDevices = HK::mapRange<QStringList>(headsetcontrol::supportedDevices(), [](std::string_view view) {
        return QAnyStringView(view).toString();
    });
    return supportedDevices;
}

QList<HKDevice *> HKHeadsetControl::devices()
{
    return static_cast<const HKHeadsetControl *>(instance())->m_worker->devices();
}

void HKHeadsetControl::start()
{
    auto *const i = instance();
    if (!i->m_running) {
        i->m_running = true;
        QMetaObject::invokeMethod(i->m_worker.get(), &HKHeadsetControlWorker::start);
    }
}

void HKHeadsetControl::stop()
{
    QMetaObject::invokeMethod(instance()->m_worker.get(), &HKHeadsetControlWorker::stop);
}

void HKHeadsetControl::update()
{
    QMetaObject::invokeMethod(instance()->m_worker.get(), &HKHeadsetControlWorker::update);
}
