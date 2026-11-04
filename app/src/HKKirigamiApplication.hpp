// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#pragma once

#include <AbstractKirigamiApplication>

class HKKirigamiApplication : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT
public:
    enum Action {
        CommandBar,
        Configure,
        ConfigureKeybinding,
        ConfigureNotifications,
        HCStart,
        HCStop,
        HCRefresh,
        About,
        BugReport,
        Quit
    };
    Q_ENUM(Action)

    explicit HKKirigamiApplication(QObject *parent = nullptr);

    QList<KirigamiActionCollection *> actionCollections() const override;

    using AbstractKirigamiApplication::action;
    [[nodiscard]] Q_INVOKABLE QAction *action(Action action);

protected:
    void setupActions() override;

private:
    QPointer<QWindow> m_window;
    KirigamiActionCollection *m_hcActionCollection;
};
