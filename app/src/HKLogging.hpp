// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include <QLoggingCategory>

#define HK_DEBUG_CATEGORY(Category, ...) qCDebug(Category __VA_OPT__(, ) __VA_ARGS__)
#define HK_INFO_CATEGORY(Category, ...) qCInfo(Category __VA_OPT__(, ) __VA_ARGS__)
#define HK_WARN_CATEGORY(Category, ...) qCWarning(Category __VA_OPT__(, ) __VA_ARGS__)
#define HK_CRIT_CATEGORY(Category, ...) qCCritical(Category __VA_OPT__(, ) __VA_ARGS__)
#define HK_FATAL_CATEGORY(Category, ...) qCFatal(Category __VA_OPT__(, ) __VA_ARGS__)

#ifndef HK_LOGGING_CATEGORY
#define HK_LOGGING_CATEGORY HK_LOGGING
#endif // HK_LOGGING_CATEGORY

Q_DECLARE_LOGGING_CATEGORY(HK_LOGGING_CATEGORY)

#define HK_DEBUG(...) HK_DEBUG_CATEGORY(HK_LOGGING_CATEGORY __VA_OPT__(, ) __VA_ARGS__)
#define HK_INFO(...) HK_INFO_CATEGORY(HK_LOGGING_CATEGORY __VA_OPT__(, ) __VA_ARGS__)
#define HK_WARN(...) HK_WARN_CATEGORY(HK_LOGGING_CATEGORY __VA_OPT__(, ) __VA_ARGS__)
#define HK_CRIT(...) HK_CRIT_CATEGORY(HK_LOGGING_CATEGORY __VA_OPT__(, ) __VA_ARGS__)
#define HK_FATAL(...) HK_FATAL_CATEGORY(HK_LOGGING_CATEGORY __VA_OPT__(, ) __VA_ARGS__)
