// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#include "HKDeviceId.hpp"

#include "HKNamespace.hpp"

#include <QDebug>

using namespace Qt::StringLiterals;

namespace
{
[[nodiscard]] QString idToString(quint16 id)
{
    return u"0x%1"_s.arg(id, 4, 16, '0'_L1).toLower();
}
}

QString HKDeviceId::toString() const
{
    return idToString(m_vendorId) + ":"_L1 + idToString(m_productId);
}

HKDeviceId HKDeviceId::fromString(const QString &idString)
{
    const auto parsedId = HK::parseUsbId(idString);
    if (!parsedId.has_value()) {
        return {};
    }
    return HKDeviceId{parsedId.value()};
}

HKDeviceId HKDeviceId::fromString(const QString &vendorIdString, const QString &productIdstring)
{
    const auto parsedVendorId = HK::parseUsbIdPart(vendorIdString);
    if (!parsedVendorId.has_value()) {
        return {};
    }

    const auto parsedProductId = HK::parseUsbIdPart(productIdstring);
    if (!parsedProductId.has_value()) {
        return {};
    }

    return HKDeviceId{parsedVendorId.value(), parsedProductId.value()};
}

QString HKDeviceId::vendorIdString() const
{
    return idToString(m_vendorId);
}

QString HKDeviceId::productIdString() const
{
    return idToString(m_productId);
}

QDebug operator<<(QDebug dbg, HKDeviceId id)
{
    QDebugStateSaver saver(dbg);
    dbg.noquote().nospace() << "HKDeviceId(" << idToString(id.vendorId()) << ", " << idToString(id.productId()) << ")";
    return dbg;
}
