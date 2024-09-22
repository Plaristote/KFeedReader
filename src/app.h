#pragma once

#include "cloudprovider.h"
#include "sharingservice.h"
#include "syncservice.h"
#include <QObject>
#include <QTimer>
#include <QUrl>

class QQuickWindow;
class FeedFolder;

class App : public QObject
{
    Q_OBJECT

    Q_PROPERTY(FeedFolder *rootFolder READ rootFolder CONSTANT)
    Q_PROPERTY(CloudProvider *cloudProvider READ cloudProvider CONSTANT)
    Q_PROPERTY(SharingService *sharingService READ sharingService CONSTANT)
    Q_PROPERTY(SyncService *syncService READ syncService CONSTANT)
public:
    App(QObject *parent = nullptr);
    ~App();

    // Restore current window geometry
    Q_INVOKABLE void restoreWindowGeometry(QQuickWindow *window, const QString &group = QStringLiteral("main")) const;
    // Save current window geometry
    Q_INVOKABLE void saveWindowGeometry(QQuickWindow *window, const QString &group = QStringLiteral("main")) const;

    FeedFolder *rootFolder() const
    {
        return m_rootFolder;
    }

    CloudProvider *cloudProvider() const
    {
        return m_cloudProvider;
    }

    SharingService *sharingService() const
    {
        return m_sharingService;
    }

    SyncService *syncService() const
    {
        return m_syncService;
    }

    static QString storagePath();

public Q_SLOTS:
    void load();
    void save();
    void stopFetching();
    void importOpml(const QUrl &filepath);
    void exportOpml(const QUrl &filepath);

private:
    CloudProvider *m_cloudProvider;
    FeedFolder *m_rootFolder;
    SharingService *m_sharingService;
    SyncService *m_syncService;
    QTimer m_autosaveTimer;
};
