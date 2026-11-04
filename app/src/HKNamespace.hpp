// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include <QMetaObject>
#include <QThread>

#define HK_APP_STATIC(Class, Name, ...)                                                                                                                        \
    class Class##Static                                                                                                                                        \
    {                                                                                                                                                          \
        Q_DISABLE_COPY_MOVE(Class##Static)                                                                                                                     \
    public:                                                                                                                                                    \
        Class *const instance;                                                                                                                                 \
                                                                                                                                                               \
        Class##Static(auto &&...args)                                                                                                                          \
            : instance{new Class(std::forward<decltype(args)>(args)...)}                                                                                       \
        {                                                                                                                                                      \
        }                                                                                                                                                      \
                                                                                                                                                               \
        ~Class##Static()                                                                                                                                       \
        {                                                                                                                                                      \
            delete instance;                                                                                                                                   \
        }                                                                                                                                                      \
    };                                                                                                                                                         \
                                                                                                                                                               \
    Q_APPLICATION_STATIC(Class##Static, Name, __VA_OPT__(, ) __VA_ARGS__)

#define HK_STATIC_D(Class) Class##Private *const d = instance()->d_func()

class QQuickWindow;
class QQuickItem;

namespace HK
{
namespace Private
{
[[nodiscard]] Qt::ConnectionType blockCallConnectionType(std::derived_from<QObject> auto *context)
{
    return QThread::currentThread() != context->thread() ? Qt::BlockingQueuedConnection : Qt::AutoConnection;
}
} // namespace Private

template<typename F, typename R, typename... Args>
concept InvocableR = std::invocable<F, Args...> && std::convertible_to<std::invoke_result_t<F, Args...>, R>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

Q_NAMESPACE

enum DeviceCapability {
    UnknownCapability = 0,
    SidetoneCapability = 1,
    BatteryStatusCapability = 1 << 1,
    NotificationSoundCapability = 1 << 2,
    LightsCapability = 1 << 3,
    InactiveTimeCapability = 1 << 4,
    ChatmixStatusCapability = 1 << 5,
    VoicePromptsCapability = 1 << 6,
    RotateToMuteCapability = 1 << 7,
    EqualizerPresetCapability = 1 << 8,
    EqualizerCapability = 1 << 9,
    ParametricEqualizerCapability = 1 << 10,
    MicrophoneMuteLedBrightnessCapability = 1 << 11,
    MicrophoneVolumeCapability = 1 << 12,
    VolumeLimiterCapability = 1 << 13,
    BluetoothWhenPoweredOnCapability = 1 << 14,
    BluetoothCallVolumeCapability = 1 << 15,
    NoiseFilterCapability = 1 << 16,
    SidetoneStatusCapability = 1 << 17,
    AncCapability = 1 << 18,
    AncStartupModeCapability = 1 << 19,
    AncButtonModesCapability = 1 << 20,
    MicrophoneAttachmentStatusCapability = 1 << 21,
    MicrophoneMuteStatusCapability = 1 << 22
};
Q_DECLARE_FLAGS(DeviceCapabilities, DeviceCapability)
Q_FLAG_NS(DeviceCapabilities)
Q_DECLARE_OPERATORS_FOR_FLAGS(DeviceCapabilities)

template<std::ranges::range RL, std::ranges::range L>
    requires std::convertible_to<std::ranges::range_value_t<L>, std::ranges::range_value_t<RL>>
[[nodiscard]] RL mapRange(const L &range)
{
    RL newRange{};
    for (const auto &item : range) {
        newRange.push_back(item);
    }
    return newRange;
}

template<std::ranges::range RL, std::ranges::range L, InvocableR<std::ranges::range_value_t<RL>, std::ranges::range_value_t<L>> F>
[[nodiscard]] RL mapRange(const L &range, F &&func)
{
    RL newRange{};
    for (const auto &item : range) {
        newRange.emplace_back(func(item));
    }
    return newRange;
}

bool blockCall(std::derived_from<QObject> auto *context, auto &&func)
{
    return QMetaObject::invokeMethod(context, std::forward<decltype(func)>(func), Private::blockCallConnectionType(context));
}

[[nodiscard]] QQuickWindow *findWindow(QObject *const item);

[[nodiscard]] QQuickWindow *findWindow(std::derived_from<QObject> auto *const item)
{
    using T = std::remove_pointer_t<decltype(item)>;
    if constexpr (std::is_base_of_v<QQuickWindow, T>) {
        return item;
    } else if constexpr (std::is_base_of_v<QQuickItem, T>) {
        return item ? item->window() : nullptr;
    } else if constexpr (std::is_base_of_v<QObject, T>) {
        return findWindow(static_cast<QObject *>(item));
    } else {
        return nullptr;
    }
}

[[nodiscard]] std::optional<quint16> parseUsbIdPart(const QString &idString);
[[nodiscard]] std::optional<std::pair<quint16, quint16>> parseUsbId(const QString &vendorIdString, const QString &productIdString);
[[nodiscard]] std::optional<std::pair<quint16, quint16>> parseUsbId(const QString &idString);

[[nodiscard]] QStringList sortStringLocaleAware(QStringList list);

template<Arithmetic T, Arithmetic A, Arithmetic B>
constexpr bool isSumExceedMax(const A &a, const B &b) noexcept
{
    constexpr auto T_max = std::numeric_limits<T>::max();

    if (std::cmp_greater(a, T_max) || std::cmp_greater(b, T_max)) {
        return true;
    }

    if constexpr (std::is_floating_point_v<T> || std::is_floating_point_v<A> || std::is_floating_point_v<B>) {
        using C = std::common_type_t<T, A, B>; // C will always be some floating point type.
        return (static_cast<C>(a) + static_cast<C>(b)) > static_cast<C>(T_max);
    } else {
        if (std::cmp_greater(b, 0)) {
            if (std::cmp_greater(a, 0) && std::cmp_greater(a, T_max - b)) {
                return true;
            }
        } else if (std::cmp_less(b, 0)) {
            // Adding a negative number decreases the sum, so it won't exceed T_max (but could exceed T_min).
            return false;
        } else {
            if (std::cmp_greater(a, T_max)) {
                return true;
            }
        }
        return false;
    }
}
} // namespace HK