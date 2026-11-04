// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include "HKDeviceId.hpp"
#include "HKLogging.hpp"
#include "HKNamespace.hpp"

#include <QFile>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTextStream>
#include <QtQmlIntegration/qqmlintegration.h>

class HKQml : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(HK)
    QML_SINGLETON
    QML_EXTENDED_NAMESPACE(HK)

    Q_PROPERTY(HKDeviceId emptyDeviceId READ emptyDeviceId CONSTANT FINAL)
    Q_PROPERTY(bool isContained READ isContained CONSTANT FINAL)
    Q_PROPERTY(bool hasUdevRules READ hasUdevRules CONSTANT FINAL)

public:
    Q_INVOKABLE [[nodiscard]] inline QStringList sortStringLocaleAware(QStringList list)
    {
        return HK::sortStringLocaleAware(list);
    }

    [[nodiscard]] inline static HKDeviceId emptyDeviceId() noexcept
    {
        return {};
    }

    [[nodiscard]] static constexpr bool isContained() noexcept
    {
#ifdef HK_CONTAINED
        return true;
#else
        return false;
#endif
    }

    [[nodiscard]] static constexpr bool hasUdevRules() noexcept
    {
#ifdef HK_UDEV_RULES
        return true;
#else
        return false;
#endif // HK_HAS_UDEV_RULES
    }

    Q_INVOKABLE [[nodiscard]] inline static QString getUdevRulesContent()
    {
#ifdef HK_UDEV_RULES
        QFile file(QStringLiteral(HK_UDEV_RULES));
        const auto opened = file.open(QFile::ReadOnly);
        if (!opened) {
            HK_CRIT().noquote().nospace() << "Failed to open UDEV rules: " << file.errorString();
            Q_ASSERT(opened);
            return {};
        }
        QTextStream stream(&file);
        return stream.readAll();
#else
        return {};
#endif // HK_UDEV_RULES
    }

    Q_INVOKABLE [[nodiscard]] inline static QQuickWindow *findWindow(QObject *item)
    {
        return HK::findWindow(item);
    }

    Q_INVOKABLE [[nodiscard]] inline static QQuickWindow *findWindow(QQuickWindow *item)
    {
        return HK::findWindow(item);
    }

    Q_INVOKABLE [[nodiscard]] inline static QQuickWindow *findWindow(QQuickItem *item)
    {
        return HK::findWindow(item);
    }
};