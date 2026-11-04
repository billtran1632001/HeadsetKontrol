// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Trần Nam Tuấn <billtran1632001@gmail.com>

#include "HKConfigUpdate.hpp"
#include "HKDeviceId.hpp"
#include "HKHeadsetControl.hpp"
#include "HKVersion.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <KirigamiAppDefaults>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    QLoggingCategory::setFilterRules(u"vn.name.billtran.headsetkontrol.*=true"_s);
#endif // QT_DEBUG

    QApplication app(argc, argv);
    KirigamiAppDefaults::apply(&app);

    KLocalizedString::setApplicationDomain("headsetkontrol");
    QCoreApplication::setOrganizationName("Bill Trần"_L1);

    bool startMinimized = false;
    bool startPaused = false;
    {
        KAboutData aboutData("headsetkontrol"_L1,
                             i18n("HeadsetKontrol"),
                             QLatin1StringView(HK_VERSION_STRING),
                             i18n("Control your headsets with HeadsetControl."),
                             KAboutLicense::GPL_V3,
                             i18n("Copyright © 2026 Trần Nam Tuấn."),
                             {},
                             "https://headsetkontrol.billtran.name.vn"_L1,
                             "https://headsetkontrol.billtran.name.vn/bugs"_L1);

        aboutData.addAuthor(i18n("Trần Nam Tuấn"),
                            i18nc("@info:credit", "Maintainer"),
                            "billtran1632001@gmail.com"_L1,
                            "https://billtran.name.vn"_L1,
                            QUrl("https://gitlab.com/uploads/-/system/user/avatar/10312183/avatar.png"_L1));

        aboutData.addComponent(
            i18nc("HeadsetControl name", "HeadsetControl"),
            i18nc("HeadsetControl description",
                  "Sidetone and Battery status for Logitech G930, G533, G633, G933 SteelSeries Arctis 7/PRO 2019 and Corsair VOID (Pro) in Linux and MacOSX."),
            HKHeadsetControl::version(),
            "https://github.com/Sapd/HeadsetControl"_L1,
            KAboutLicense::GPL_V3);

        aboutData.addCredit(i18nc("name of HeadsetControl author", "Denis Arnst"),
                            i18nc("HeadsetControl author task", "HeadsetControl author and maintainer"),
                            "git@sapd.eu"_L1,
                            "https://blog.arnst.cc"_L1,
                            QUrl("https://avatars.githubusercontent.com/u/671752"_L1));

        // This should be filled in the translation files.
        aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

        aboutData.setDesktopFileName("vn.name.billtran.headsetkontrol"_L1);
        aboutData.setOrganizationDomain("billtran.name.vn"_ba);

        KAboutData::setApplicationData(aboutData);

        QCommandLineParser parser;

        QCommandLineOption minimizedOption(QStringList{"m"_L1, "minimized"_L1}, i18nc("@info:shell argument help message", "Start application minimized."));
        parser.addOption(minimizedOption);

        QCommandLineOption pausedOption(QStringList{"p"_L1, "paused"_L1},
                                        i18nc("@info:shell argument help message", "Start application with the auto-refresh paused."));
        parser.addOption(pausedOption);

        aboutData.setupCommandLine(&parser);
        parser.process(app);
        aboutData.processCommandLine(&parser);

        startMinimized = parser.isSet(minimizedOption);
        startPaused = parser.isSet(pausedOption);
    }

    KCrash::initialize();

    HKConfigUpdate::update();

    QApplication::setWindowIcon(QIcon::fromTheme("headsetkontrol"_L1));

    QMetaType::registerConverter<HKDeviceId, QString>([](HKDeviceId id) {
        return id.toString();
    });

    QMetaType::registerConverter<QString, HKDeviceId>([](const QString &str) {
        return HKDeviceId::fromString(str);
    });

    if (!startPaused) {
        HKHeadsetControl::start();
    }

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);

    engine.setInitialProperties({{"visible"_L1, !startMinimized}});

    // BUG: https://bugs.kde.org/show_bug.cgi?id=527573
    // if (!KirigamiAppDefaults::load("vn.name.billtran.headsetkontrol", "HKAppWindow", &engine)) {
    //     return EXIT_FAILURE;
    // }
    engine.loadFromModule("vn.name.billtran.headsetkontrol", "HKAppWindow");
    if (engine.rootObjects().isEmpty()) {
        return EXIT_FAILURE;
    }

    return app.exec();
}
