// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#include "HKDevice.hpp"
#include "HKDevice_p.hpp"

#include "HKHeadsetControl_p.hpp"

#define HK_LOGGING_CATEGORY HK_HC_LOGGING
#include "HKLogging.hpp"

#include <QThreadPool>

namespace
{
constexpr HK::DeviceCapability toCapability(enum capabilities cap) noexcept
{
    switch (cap) {
    case CAP_SIDETONE:
        return HK::SidetoneCapability;
    case CAP_BATTERY_STATUS:
        return HK::BatteryStatusCapability;
    case CAP_NOTIFICATION_SOUND:
        return HK::NotificationSoundCapability;
    case CAP_LIGHTS:
        return HK::LightsCapability;
    case CAP_INACTIVE_TIME:
        return HK::InactiveTimeCapability;
    case CAP_CHATMIX_STATUS:
        return HK::ChatmixStatusCapability;
    case CAP_VOICE_PROMPTS:
        return HK::VoicePromptsCapability;
    case CAP_ROTATE_TO_MUTE:
        return HK::RotateToMuteCapability;
    case CAP_EQUALIZER_PRESET:
        return HK::EqualizerPresetCapability;
    case CAP_EQUALIZER:
        return HK::EqualizerCapability;
    case CAP_PARAMETRIC_EQUALIZER:
        return HK::ParametricEqualizerCapability;
    case CAP_MICROPHONE_MUTE_LED_BRIGHTNESS:
        return HK::MicrophoneMuteLedBrightnessCapability;
    case CAP_MICROPHONE_VOLUME:
        return HK::MicrophoneVolumeCapability;
    case CAP_VOLUME_LIMITER:
        return HK::VolumeLimiterCapability;
    case CAP_BT_WHEN_POWERED_ON:
        return HK::BluetoothWhenPoweredOnCapability;
    case CAP_BT_CALL_VOLUME:
        return HK::BluetoothCallVolumeCapability;
    case CAP_NOISE_FILTER:
        return HK::NoiseFilterCapability;
    case CAP_SIDETONE_STATUS:
        return HK::SidetoneStatusCapability;
    case CAP_ANC:
        return HK::AncCapability;
    case CAP_ANC_STARTUP_MODE:
        return HK::AncStartupModeCapability;
    case CAP_ANC_BUTTON_MODES:
        return HK::AncButtonModesCapability;
    case CAP_MICROPHONE_ATTACHMENT_STATUS:
        return HK::MicrophoneAttachmentStatusCapability;
    case CAP_MICROPHONE_MUTE_STATUS:
        return HK::MicrophoneMuteStatusCapability;
    default:
        break;
    }
    return HK::UnknownCapability;
}

constexpr HK::DeviceCapabilities toCapabilities(int mask) noexcept
{
    HK::DeviceCapabilities caps;
    for (std::underlying_type_t<enum capabilities> i = 0; i < capabilities::NUM_CAPABILITIES; ++i) {
        if (has_capability(mask, static_cast<enum capabilities>(i))) {
            caps.setFlag(toCapability(static_cast<enum capabilities>(i)));
        }
    }
    return caps;
}
}

HKDeviceWorker::HKDeviceWorker(std::vector<headsetcontrol::Headset> &&hcHeadsets, HKDevice *device, HKHeadsetControlWorker *parent)
    : QObject{parent}
    , info{{hcHeadsets.front().vendorId(), hcHeadsets.front().productId()},
           QUtf8StringView(hcHeadsets.front().name()).toString(),
           QUtf8StringView(hcHeadsets.front().vendorName()).toString(),
           QUtf8StringView(hcHeadsets.front().productName()).toString()}
    , m_parentWorker{parent}
    , m_device{device}
    , m_hcHeadsets{std::move(hcHeadsets)}
{
}

bool HKDeviceWorker::doUpdate()
{
    bool updated = false;

    if (m_hcHeadsets.empty()) {
        return updated;
    }

    auto &headset = m_hcHeadsets.front();

    const auto capabilities = toCapabilities(headset.capabilitiesMask());

    HK::blockCall(m_device, [this, &updated, capabilities] {
        if (m_device->capabilities() != capabilities) {
            updated = true;
            m_device->m_capabilities = capabilities;
        }
    });

#ifndef QT_NO_DEBUG_OUTPUT
    if (updated) {
        HK_DEBUG().noquote().nospace() << "Device updated: " << info.productName << " [" << info.id.toString() << "]";
    }
#endif // QT_NO_DEBUG_OUTPUT

    return updated;
}

HKDevice::HKDevice(HKDeviceInit &&init, HKHeadsetControl *parent)
    : m_parent{parent}
    , m_worker{std::make_unique<HKDeviceWorker>(std::move(init).headsets, this, init.parent)}
{
    moveToThread(parent->thread());
    setParent(parent);
}

HKDevice::~HKDevice() = default;

HKDeviceId HKDevice::id() const
{
    return m_worker->info.id;
}

QString HKDevice::name() const
{
    return m_worker->info.name;
}

QString HKDevice::vendorName() const
{
    return m_worker->info.vendorName;
}

QString HKDevice::productName() const
{
    return m_worker->info.productName;
}

void HKDevice::update()
{
    QMetaObject::invokeMethod(m_worker.get(), [this] {
        if (m_worker->doUpdate()) {
            Q_EMIT m_parent->deviceUpdated(this);
            Q_EMIT m_parent->devicesUpdated({this});
        }
    });
}
