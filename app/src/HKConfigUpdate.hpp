// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

namespace HKConfigUpdate
{
/*
 * \brief Perform config migration.
 *
 * kconf_update is portable so we will have to roll our own.
 */
void update();
} // namespace HKConfigUpdate
