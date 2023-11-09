// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

#include "app.h"
#include "feedfolder.h"
#include <KSharedConfig>
#include <KWindowConfig>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuickWindow>
#include <QStandardPaths>

App::App(QObject *parent)
    : QObject(parent)
{
    m_rootFolder = new FeedFolder(this);
}

App::~App()
{
    save();
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

#include "moc_app.cpp"
