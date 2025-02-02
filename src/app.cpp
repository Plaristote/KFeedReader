#include "app.h"
#include "feed.h"
#include "feedfetch.h"
#include "feedfolder.h"
#include "javascriptplugins.h"
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuickWindow>
#include <QStandardPaths>

void importOpmlDocumentIn(FeedFolder *folder, QDomDocument &document);
void exportAsOpmlDocument(const FeedFolder *folder, QDomDocument &document);

App::App(QObject *parent)
    : QObject(parent)
{
    m_javaScriptPlugins = new JavaScriptPlugins();
    m_sharingService = new SharingService();
    m_rootFolder = new FeedFolder();
    m_autosaveTimer.setInterval(1000 * 60 * 90); // 1.5h but I couldnt get chrono literals to work
    m_autosaveTimer.start();
    connect(&m_autosaveTimer, &QTimer::timeout, this, &App::save);
}

App::~App()
{
    save();
    m_javaScriptPlugins->deleteLater();
    m_sharingService->deleteLater();
    m_rootFolder->deleteLater();
}

void App::restoreWindowGeometry(QQuickWindow *window, const QString &group) const
{
    KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
    KConfigGroup windowGroup(&dataResource, QStringLiteral("Window-") + group);
    KWindowConfig::restoreWindowSize(window, windowGroup);
    KWindowConfig::restoreWindowPosition(window, windowGroup);
}

void App::saveWindowGeometry(QQuickWindow *window, const QString &group) const
{
    KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
    KConfigGroup windowGroup(&dataResource, QStringLiteral("Window-") + group);
    KWindowConfig::saveWindowPosition(window, windowGroup);
    KWindowConfig::saveWindowSize(window, windowGroup);
    dataResource.sync();
}

QString App::storagePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/feeds.json");
}

QString App::backupStoragePath()
{
    return storagePath() + QStringLiteral(".backup");
}

void App::load()
{
    QFile file(storagePath());

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = document.object();

        m_rootFolder->loadFromJson(root);
        QFile::copy(storagePath(), backupStoragePath());
    } else
        qDebug() << "App::load: failed to open" << storagePath();
}

void App::save()
{
    QFile file(storagePath());

    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject root;

        m_rootFolder->triggerBeforeSave();
        m_rootFolder->saveToJson(root);
        if (root[QStringLiteral("items")].toArray().size() > 0)
            file.write(QJsonDocument(root).toJson());
        else
            qDebug() << "App::save: not writing to disk, items array is empty";
    } else
        qDebug() << "App::save: failed to open" << storagePath();
}

void App::importOpml(const QUrl &filepath)
{
    QFile file(filepath.path());

    if (file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        QDomElement base;

        document.setContent(file.readAll());
        importOpmlDocumentIn(m_rootFolder, document);
    }
}

void App::exportOpml(const QUrl &filepath)
{
    QFile file(filepath.path());

    if (file.open(QIODevice::WriteOnly)) {
        QDomDocument document;

        exportAsOpmlDocument(m_rootFolder, document);
        file.write(QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n").toUtf8());
        file.write(document.toString().toUtf8());
    }
}

void App::stopFetching()
{
    FeedFetcher::interrupt();
}

#include "moc_app.cpp"
