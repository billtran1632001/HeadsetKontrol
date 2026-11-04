// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "HKDeviceInfo.hpp"

#include "HKDevice_p.hpp"

HKDeviceInfo::HKDeviceInfo(HKDeviceId id, QString name, QString vendorName, QString productName)
    : id{id}
    , name{std::move(name)}
    , vendorName{std::move(vendorName)}
    , productName{std::move(productName)}
{
}

HKDeviceInfo::HKDeviceInfo(const HKDevice *const device)
    : HKDeviceInfo{device->m_worker->info}
{
}
