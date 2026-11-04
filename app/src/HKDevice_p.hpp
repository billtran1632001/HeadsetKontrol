// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-later

#pragma once

#include "HKDevice.hpp" // IWYU pragma: export
#include "HKDeviceInfo.hpp"

#include "headsetcontrol.hpp"

class HKHeadsetControlWorker;

struct HKDeviceInit {
    std::vector<headsetcontrol::Headset> headsets;
    HKHeadsetControlWorker *parent;
};

class HKDeviceWorker : public QObject
{
    Q_OBJECT
    friend HKHeadsetControlWorker;

public:
    explicit HKDeviceWorker(std::vector<headsetcontrol::Headset> &&hcHeadsets, HKDevice *device, HKHeadsetControlWorker *parent);

    const HKDeviceInfo info;

    // Always call these methods in HKDeviceWorker's thread
    [[maybe_unused]] bool doUpdate();

private:
    HKHeadsetControlWorker *const m_parentWorker;
    HKDevice *const m_device;

    std::vector<headsetcontrol::Headset> m_hcHeadsets;
};