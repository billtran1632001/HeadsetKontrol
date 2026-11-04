// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "HKConfigUpdate.hpp"

#include "HKVersion.h"

#include "HKConfig.hpp"
#include "HKDeviceId.hpp"
#include "HKLogging.hpp"

#include <QVersionNumber>

using namespace Qt::StringLiterals;

namespace HKConfigUpdate
{
namespace
{
struct Version {
    int major{0};
    int minor{0};
    int patch{0};

    operator QVersionNumber() const
    {
        return QVersionNumber(major, minor, patch);
    }
};

void update_3_0_0();

// Always keep this sorted.
constexpr std::array updates{std::make_pair(Version{3, 0, 0}, &update_3_0_0)};
}

void update()
{
    const auto version = QVersionNumber::fromString(HKConfig::metaVersion());
    const QVersionNumber currentVersion(HK_VERSION_MAJOR, HK_VERSION_MINOR, HK_VERSION_PATCH);

    if (version > currentVersion) {
        HK_WARN("Config file version '%s' is greater than current version '%s'.",
                qUtf8Printable(version.toString()),
                qUtf8Printable(currentVersion.toString()));
        return;
    }

    if (version == currentVersion) {
        return;
    }

    for (const auto &pair : updates) {
        if (pair.first > currentVersion) {
            break;
        }

        HK_INFO("Migrating config file to version '%s'...", qUtf8Printable(QVersionNumber(pair.first).toString()));

        pair.second();
    }

    HKConfig::setMetaVersion(currentVersion.toString());
    HKConfig::self()->save();
}

namespace
{
void update_3_0_0()
{
    const auto config = HKConfig::self()->sharedConfig();

    auto devicesGroup = config->group("Devices"_L1);

    {
        auto generalGroup = config->group("General"_L1);
        auto userInterfaceGroup = config->group("UserInterface"_L1);
        auto headsetControlGroup = config->group("HeadsetControl"_L1);

        if (generalGroup.hasKey("Primary"_L1)) {
            const auto list = generalGroup.readEntry("Primary"_L1, QStringList{});

            bool check = false;
            if (list.size() < 2) {
                const auto deviceId = HKDeviceId::fromString(list[0], list[1]);
                if (deviceId.isValid()) {
                    devicesGroup.writeEntry("PrimaryDevice"_L1, deviceId.toString());
                    check = true;
                }
            }

            if (!check) {
                HK_WARN() << "Invalid 'Primary' entry:" << list;
            }

            generalGroup.deleteEntry("Primary"_L1);
        }

        if (userInterfaceGroup.hasKey(u"ShowCountdownProgress"_s)) {
            const auto value = userInterfaceGroup.readEntry(u"ShowCountdownProgress"_s, HKConfig::defaultShowRefreshTimerValue());
            if (value != HKConfig::defaultShowRefreshTimerValue()) {
                userInterfaceGroup.writeEntry(u"showRefreshTimer"_s, value);
            }

            userInterfaceGroup.deleteEntry(u"ShowCountdownProgress"_s);
        }

        if (headsetControlGroup.hasKey("UpdateRate"_L1)) {
            headsetControlGroup.writeEntry("UpdateDelay"_L1, headsetControlGroup.readEntry("UpdateRate"_L1, HKConfig::defaultUpdateDelayValue()));
            headsetControlGroup.deleteEntry("UpdateRate"_L1);
        }

        headsetControlGroup.deleteEntry(u"ExecutablePath"_s);
    }

    const auto vendorGroups = config->groupList();
    for (const auto &vendorGroupName : vendorGroups) {
        if (!vendorGroupName.startsWith("0x"_L1)) {
            continue;
        }

        auto vendorGroup = config->group(vendorGroupName);
        const auto productGroups = vendorGroup.groupList();

        for (const auto &productGroupName : productGroups) {
            if (!productGroupName.startsWith("0x"_L1)) {
                continue;
            }

            auto productGroup = vendorGroup.group(productGroupName);

            const auto deviceId = HKDeviceId::fromString(vendorGroupName, productGroupName);
            if (!deviceId.isValid()) {
                continue;
            }

            auto deviceGroup = devicesGroup.group(deviceId.toString());
            productGroup.moveValuesTo(deviceGroup);

            if (deviceGroup.hasKey("MicrophonBrightness"_L1)) {
                deviceGroup.writeEntry("MicrophoneMuteLEDBrightness"_L1, deviceGroup.readEntry("MicrophonBrightness"_L1, 0));
                deviceGroup.deleteEntry("MicrophonBrightness"_L1);
            }

            if (deviceGroup.hasKey("Equalizer"_L1)) {
                deviceGroup.writeEntry("Equalizer"_L1, deviceGroup.readEntry("Equalizer"_L1).split(','_L1, Qt::SkipEmptyParts));
            }
        }

        config->deleteGroup(vendorGroupName);
    }

    config->sync();
}
}
} // namespace HKConfigUpdate
