// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

#include "app.h"
#include "feed.h"
#include "feedfolder.h"
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuickWindow>
#include <QStandardPaths>

App::App(QObject *parent)
    : QObject(parent)
{
    m_rootFolder = new FeedFolder();
}

App::~App()
{
    save();
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

void App::load()
{
    QFile file(storagePath());

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = document.object();

        m_rootFolder->loadFromJson(root);
    } else
        qDebug() << "App::load: failed to open" << storagePath();
}

void App::save()
{
    QFile file(storagePath());

    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject root;

        m_rootFolder->saveToJson(root);
        file.write(QJsonDocument(root).toJson());
    } else
        qDebug() << "App::save: failed to open" << storagePath();
}

static void importOpmlOutlineIn(FeedFolder *folder, QDomElement root)
{
    const QString itemTag = QStringLiteral("outline");
    const QString xmlUrlAttribute = QStringLiteral("xmlUrl");
    const QString linkAttribute = QStringLiteral("link");
    const QString nameAttribute = QStringLiteral("text");
    const QString descriptionAttribute = QStringLiteral("comment");
    const QString idAttribute = QStringLiteral("id");

    for (QDomElement outline = root.firstChildElement(itemTag); !outline.isNull(); outline = outline.nextSiblingElement(itemTag)) {
        MenuItem *item = nullptr;

        qDebug() << "Adding an tem frir";
        if (outline.hasAttribute(xmlUrlAttribute)) {
            Feed *feed = new Feed(folder);

            item = feed;
            if (outline.hasAttribute(idAttribute) && !outline.attribute(idAttribute).isEmpty())
                feed->setUuid(outline.attribute(idAttribute));
            feed->setXmlUrl(QUrl(outline.attribute(xmlUrlAttribute)));
            feed->setLink(QUrl(outline.attribute(linkAttribute)));
        } else {
            FeedFolder *subfolder = new FeedFolder(folder);

            item = subfolder;
            importOpmlOutlineIn(subfolder, outline);
        }
        item->setName(outline.attribute(nameAttribute));
        item->setDescription(outline.attribute(descriptionAttribute));
        folder->addItem(item);
    }
}

void App::importOpml(const QUrl &filepath)
{
    QFile file(filepath.path());

    qDebug() << "IMPORT OPML" << filepath.toString();
    if (file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        QDomElement base;

        document.setContent(file.readAll());
        base = document.firstChildElement(QStringLiteral("opml")).firstChildElement(QStringLiteral("body"));
        qDebug() << "OPML BASE EXISTS" << base.isNull();
        importOpmlOutlineIn(m_rootFolder, base);
    } else
        qDebug() << "Could not open OPML file";
}

void App::exportOpml(const QUrl &filepath)
{
    // TODO
}

#include "moc_app.cpp"
