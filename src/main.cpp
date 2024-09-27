#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QResource>
#include <QUrl>
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include "app.h"
#include "version-fluxkap.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "aggregatedfeed.h"
#include "feed.h"
#include "feedarticle.h"
#include "feedfolder.h"
#include "fluxkapconfig.h"
#ifdef APP_WEBENGINE_DISABLED
#include <QtWebView>
#else
#include <QQuickWebEngineProfile>
#include <QtWebEngineQuick/qtwebenginequickglobal.h>
#endif

#ifdef Q_OS_WINDOWS
#include <QDebug>
#include <iostream>
#include <windows.h>

static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    std::cout << msg.toStdString() << std::endl;
}
#endif

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif

int main(int argc, char *argv[])
{
#ifdef APP_WEBENGINE_DISABLED
    QtWebView::initialize();
#else
    QtWebEngineQuick::initialize();
#endif

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
    qInstallMessageHandler(myMessageOutput);
    /*
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    */

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif

    KLocalizedString::setApplicationDomain("FluxKap");
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));

#ifdef APP_WEBENGINE_DISABLED
    QResource::registerResource(QStringLiteral("webview.rcc"));
#else
    QResource::registerResource(QStringLiteral("webengine.rcc"));
#endif

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("kfeedreader"),
        // A displayable program name string.
        i18nc("@title", "FluxKap"),
        // The program version string.
        QStringLiteral(FLUXKAP_VERSION_STRING),
        // Short description of what the app does.
        i18n("RSS feed aggregator"),
        // The license this code is released under.
        KAboutLicense::GPL,
        // Copyright Statement.
        i18n("(c) 2024"));
    aboutData.addAuthor(i18nc("@info:credit", "Michael Martin Moro"),
                        i18nc("@info:credit", "Developer"),
                        QStringLiteral("michael@unetresgrossebite.com"),
                        QStringLiteral("https://github.com/Plaristote"));
    aboutData.addCredit(i18nc("@info:credit", "Louis Schul"),
                        i18nc("@info:credit", "Developer"),
                        QString(),
                        QStringLiteral("https://invent.kde.org/louis-schul"));
    // aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/icon.png")));

#ifndef APP_WEBENGINE_DISABLED
    QQuickWebEngineProfile *webEngineProfile = QQuickWebEngineProfile::defaultProfile();
    webEngineProfile->setPersistentCookiesPolicy(QQuickWebEngineProfile::ForcePersistentCookies);
    webEngineProfile->setOffTheRecord(false);
    webEngineProfile->setStorageName(QStringLiteral("FluxKap"));
    webEngineProfile->setCachePath(webEngineProfile->persistentStoragePath() + QStringLiteral("/Cache"));
#endif

    QQmlApplicationEngine engine;

    auto config = FluxKapConfig::self();

    qmlRegisterSingletonInstance("org.kde.fluxkap", 1, 0, "Config", config);

    qmlRegisterSingletonType("org.kde.fluxkap", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    App application;
    qmlRegisterSingletonInstance("org.kde.fluxkap", 1, 0, "App", &application);

    qmlRegisterType<MenuItem>("org.kde.fluxkap", 1, 0, "MenuItem");
    qmlRegisterType<FeedFolder>("org.kde.fluxkap", 1, 0, "FeedFolder");
    qmlRegisterType<FeedArticle>("org.kde.fluxkap", 1, 0, "FeedArticle");
    qmlRegisterType<Feed>("org.kde.fluxkap", 1, 0, "Feed");
    qmlRegisterUncreatableType<FeedUpdater>("org.kde.fluxkap", 1, 0, "FeedUpdater", QStringLiteral("access to enums and static methods only"));
    qmlRegisterType<AggregatedFeed>("org.kde.fluxkap", 1, 0, "AggregatedFeed");
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
