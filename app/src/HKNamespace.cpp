// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "HKNamespace.hpp"
#include "HKNamespace_qml.hpp" // IWYU pragma: keep

#include "HKLogging.hpp"

#include <QCollator>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRegularExpression>
#include <QThread>

using namespace Qt::StringLiterals;

namespace HK
{
namespace
{
[[nodiscard]] const QRegularExpression &udbIdPartRegex()
{
    static const QRegularExpression regex(QStringLiteral(R"(^(0x[0-9a-fA-F]{4})$)"));
    return regex;
}
}

QQuickWindow *findWindow(QObject *const item)
{
    QObject *current = item;
    while (current) {
        if (auto *const window = qobject_cast<QQuickWindow *>(current)) {
            return window;
        }
        if (auto *const quickItem = qobject_cast<QQuickItem *>(current)) {
            return quickItem->window();
        }
        current = item->parent();
    }
    return nullptr;
}

std::optional<quint16> parseUsbIdPart(const QString &idString)
{
    const auto match = udbIdPartRegex().match(idString);
    if (!match.hasCaptured(1)) {
        HK_WARN("Failed to parse USB ID part '%s': Invalid format.", qUtf8Printable(idString));
        return {};
    }

    const auto hexStr = match.captured(1);

    bool check = false;

    auto value = hexStr.toUShort(&check, 16);
    if (!check) {
        HK_WARN("Failed to parse USB ID part '%s': Conversion failed.", qUtf8Printable(idString));
        return {};
    }

    return value;
}

std::optional<std::pair<quint16, quint16>> parseUsbId(const QString &vendorIdString, const QString &productIdString)
{
    const auto vendorIdMatch = udbIdPartRegex().match(vendorIdString);
    const auto productIdMatch = udbIdPartRegex().match(productIdString);

    if (!vendorIdMatch.hasCaptured(1) || !productIdMatch.hasCaptured(1)) {
        HK_WARN("Failed to parse USB ID string '%s:%s': Invalid format.", qUtf8Printable(vendorIdString), qUtf8Printable(productIdString));
        return {};
    }

    const auto vendorIdHexStr = vendorIdMatch.captured(1);
    const auto productIdHexStr = productIdMatch.captured(1);

    bool check = false;

    const quint16 vendorId = vendorIdHexStr.toUShort(&check, 16);
    if (!check) {
        HK_WARN("Failed to parse USB ID string '%s:%s': Vendor ID conversion failed.", qUtf8Printable(vendorIdString), qUtf8Printable(productIdString));
        return {};
    }

    const quint16 productId = productIdHexStr.toUShort(&check, 16);
    if (!check) {
        HK_WARN("Failed to parse USB ID string '%s:%s': Product ID conversion failed.", qUtf8Printable(vendorIdString), qUtf8Printable(productIdString));
        return {};
    }

    return std::make_pair(vendorId, productId);
}

std::optional<std::pair<quint16, quint16>> parseUsbId(const QString &idString)
{
    const auto strList = idString.split(':'_L1);
    if (strList.size() != 2) {
        HK_WARN("Failed to parse USB ID string '%s': Invalid format.", qUtf8Printable(idString));
        return {};
    }

    return parseUsbId(strList[0], strList[1]);
}

QStringList sortStringLocaleAware(QStringList list)
{
    QCollator order;
    std::ranges::sort(list, order);
    return list;
}
} // namespace HK
