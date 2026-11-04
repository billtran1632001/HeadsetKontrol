// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#pragma once

#include <QMetaType>
#include <QtQmlIntegration/qqmlintegration.h>

class HKDeviceId
{
    Q_GADGET
    QML_VALUE_TYPE(hkDeviceId)

    Q_PROPERTY(quint16 vendorId READ vendorId CONSTANT FINAL)
    Q_PROPERTY(quint16 productId READ productId CONSTANT FINAL)
    Q_PROPERTY(bool valid READ isValid CONSTANT FINAL)
public:
    constexpr HKDeviceId() noexcept = default;

    constexpr HKDeviceId(quint16 vendorId, quint16 productId) noexcept
        : m_vendorId{vendorId}
        , m_productId{productId}
    {
    }

    constexpr HKDeviceId(std::pair<quint16, quint16> pair) noexcept
        : m_vendorId{pair.first}
        , m_productId{pair.second}
    {
    }

    [[nodiscard]] constexpr quint16 vendorId() const noexcept
    {
        return m_vendorId;
    }

    [[nodiscard]] constexpr quint16 productId() const noexcept
    {
        return m_productId;
    }

    [[nodiscard]] constexpr bool isValid() const noexcept
    {
        return m_vendorId != 0 && m_productId != 0;
    }

    [[nodiscard]] Q_INVOKABLE QString toString() const;

    [[nodiscard]] Q_INVOKABLE QString vendorIdString() const;

    [[nodiscard]] Q_INVOKABLE QString productIdString() const;

    [[nodiscard]] static HKDeviceId fromString(const QString &idString);

    [[nodiscard]] static HKDeviceId fromString(const QString &vendorIdString, const QString &productIdstring);

    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const HKDeviceId &lhs, const HKDeviceId &rhs) noexcept
    {
        auto order = lhs.m_vendorId <=> rhs.m_vendorId;
        if (order == std::strong_ordering::equal) {
            order = lhs.m_productId <=> rhs.m_productId;
        }
        return order;
    }

    [[nodiscard]] friend constexpr bool operator==(const HKDeviceId &lhs, const HKDeviceId &rhs) noexcept = default;

private:
    quint16 m_vendorId{0};
    quint16 m_productId{0};
};

Q_DECLARE_METATYPE(HKDeviceId)
Q_DECLARE_TYPEINFO(HKDeviceId, Q_PRIMITIVE_TYPE);

template<>
struct std::hash<HKDeviceId> {
    constexpr std::size_t operator()(HKDeviceId id, [[maybe_unused]] std::size_t seed = 0) const noexcept
    {
        return (quint32(id.vendorId()) << 16U) | quint32(id.productId());
    }
};

QDebug operator<<(QDebug dbg, HKDeviceId id);
