// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "device_registry.hpp"

#include <print>

// Basically copy from cli/main.cpp#printUdevRules()

int main()
{
    auto &registry = headsetcontrol::DeviceRegistry::instance();
    registry.initialize();

    std::println("ACTION!=\"add|change\", GOTO=\"headset_end\"");
    std::println();

    for (const auto &device_ptr : registry.getAllDevices()) {
        auto *device = device_ptr.get();
        println("# {}", device->getDeviceName());
        for (uint16_t pid : device->getProductIds()) {
            std::println(
                "KERNEL==\"hidraw*\", SUBSYSTEM==\"hidraw\", "
                "ATTRS{{idVendor}}==\"{:04x}\", ATTRS{{idProduct}}==\"{:04x}\", TAG+=\"uaccess\"",
                device->getVendorId(),
                pid);
        }
        std::println();
    }
    std::println("LABEL=\"headset_end\"");

    return EXIT_SUCCESS;
}