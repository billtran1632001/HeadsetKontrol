// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "HKKirigamiApplication.hpp"

#include "HKHeadsetControl.hpp"

#include <QDesktopServices>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>

#include <KAboutData>
#include <KAuthorized>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

HKKirigamiApplication::HKKirigamiApplication(QObject *parent)
    : AbstractKirigamiApplication{parent}
    , m_hcActionCollection{new KirigamiActionCollection(this, "HeasetControl"_L1)}
{
    HKKirigamiApplication::setupActions();
}

QList<KirigamiActionCollection *> HKKirigamiApplication::actionCollections() const
{
    return AbstractKirigamiApplication::actionCollections() << m_hcActionCollection;
}

QAction *HKKirigamiApplication::action(Action actionKey)
{
    switch (actionKey) {
    case CommandBar:
        return action("open_kcommand_bar"_L1);
    case Configure:
        return action("options_configure"_L1);
    case ConfigureKeybinding:
        return action("options_configure_keybinding"_L1);
    case About:
        return action("open_about_page"_L1);
    case Quit:
        return action("file_quit"_L1);
    case HCStart:
        return action("hc_start"_L1);
    case HCStop:
        return action("hc_stop"_L1);
    case HCRefresh:
        return action("hc_refresh"_L1);
    case BugReport:
        return action(KStandardActions::name(KStandardActions::ReportBug));
    }
    return nullptr;
}

void HKKirigamiApplication::setupActions()
{
    AbstractKirigamiApplication::setupActions();

    auto actionName = KStandardActions::name(KStandardActions::ReportBug);
    if (KAuthorized::authorizeAction(actionName)) {
        auto reportBugAction =
            KStandardActions::reportBug(this, std::bind_front(&QDesktopServices::openUrl, QUrl(KAboutData::applicationData().bugAddress())), mainCollection());

        mainCollection()->addAction(reportBugAction->objectName(), reportBugAction);
    }

    auto *const hcInstance = HKHeadsetControl::instance();

    actionName = "hc_start"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto *hcStartAction = m_hcActionCollection->addAction(actionName, hcInstance, &HKHeadsetControl::start);

        hcStartAction->setText(i18nc("@action start update headsetcontrol", "Start"));
        hcStartAction->setIcon(QIcon::fromTheme("media-playback-start"_L1));
        hcStartAction->setDisabled(HKHeadsetControl::isRunning());

        m_hcActionCollection->addAction(hcStartAction->objectName(), hcStartAction);

        connect(hcInstance, &HKHeadsetControl::runningChanged, hcStartAction, &QAction::setDisabled);
    }

    actionName = "hc_stop"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto *hcStopAction = m_hcActionCollection->addAction(actionName, hcInstance, &HKHeadsetControl::stop);
        hcStopAction->setText(i18nc("@action stop update headsetcontrol", "Stop"));
        hcStopAction->setIcon(QIcon::fromTheme("media-playback-stop"_L1));
        hcStopAction->setDisabled(!HKHeadsetControl::isRunning());

        m_hcActionCollection->addAction(hcStopAction->objectName(), hcStopAction);

        connect(hcInstance, &HKHeadsetControl::runningChanged, hcStopAction, &QAction::setEnabled);
    }

    actionName = "hc_refresh"_L1;
    if (KAuthorized::authorizeAction(actionName)) {
        auto hcRefreshAction = m_hcActionCollection->addAction(actionName, hcInstance, &HKHeadsetControl::update);
        hcRefreshAction->setText(i18nc("@action refresh headsetcontrol", "Refresh"));
        hcRefreshAction->setIcon(QIcon::fromTheme("view-refresh"_L1));

        m_hcActionCollection->addAction(hcRefreshAction->objectName(), hcRefreshAction);
        m_hcActionCollection->setDefaultShortcut(hcRefreshAction, Qt::Key_F5);
    }

    readSettings();
}
