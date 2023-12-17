// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>
#ifdef Q_OS_ANDROID
#include <GuiQApplication>
#else
#include <QApplication>
#endif

#include "app.h"
#include "version-kfeedreader.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "aggregatedfeed.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedfolder.h"
#include "kfeedreaderconfig.h"
#include "menuitemmodel.h"
#include <QtWebEngine/qtwebengineglobal.h>

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtWebEngine::initialize();

#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#else
    QApplication app(argc, argv);

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif

    KLocalizedString::setApplicationDomain("kfeedreader");
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("kfeedreader"),
        // A displayable program name string.
        i18nc("@title", "KFeedReader"),
        // The program version string.
        QStringLiteral(KFEEDREADER_VERSION_STRING),
        // Short description of what the app does.
        i18n("RSS feed aggregator"),
        // The license this code is released under.
        KAboutLicense::GPL,
        // Copyright Statement.
        i18n("(c) %{CURRENT_YEAR}"));
    aboutData.addAuthor(i18nc("@info:credit", "Michael Martin Moro"),
                        i18nc("@info:credit", "Developer"),
                        QStringLiteral("michael@planed.es"),
                        QStringLiteral("https://github.com/Plaristote"));
    // aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("akregator")));

    QQmlApplicationEngine engine;

    auto config = KFeedReaderConfig::self();

    qmlRegisterSingletonInstance("org.kde.kfeedreader", 1, 0, "Config", config);

    qmlRegisterSingletonType("org.kde.kfeedreader", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    App application;
    qmlRegisterSingletonInstance("org.kde.kfeedreader", 1, 0, "App", &application);

    qmlRegisterType<MenuItemModel>("org.kde.kfeedreader", 1, 0, "MenuItemModel");
    qmlRegisterType<MenuItem>("org.kde.kfeedreader", 1, 0, "MenuItem");
    qmlRegisterType<FeedFolder>("org.kde.kfeedreader", 1, 0, "FeedFolder");
    qmlRegisterType<FeedArticle>("org.kde.kfeedreader", 1, 0, "FeedArticle");
    qmlRegisterType<Feed>("org.kde.kfeedreader", 1, 0, "Feed");
    qmlRegisterUncreatableType<FeedUpdater>("org.kde.kfeedreader", 1, 0, "FeedUpdater", QStringLiteral("access to enums and static methods only"));
    qmlRegisterType<AggregatedFeed>("org.kde.kfeedreader", 1, 0, "AggregatedFeed");
    qRegisterMetaType<Feed *>("const Feed*");
    qRegisterMetaType<FeedArticle *>("const FeedArticle*");

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    application.load();

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
