#pragma once

#include "sharingservice.h"
#include <QObject>
#include <QTimer>
#include <QUrl>

class QQuickWindow;
class FeedFolder;

class App : public QObject
{
    Q_OBJECT

    Q_PROPERTY(FeedFolder *rootFolder READ rootFolder CONSTANT)
    Q_PROPERTY(SharingService *sharingService READ sharingService CONSTANT)
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

    SharingService *sharingService() const
    {
        return m_sharingService;
    }

    static QString storagePath();

public Q_SLOTS:
    void load();
    void save();
    void stopFetching();
    void importOpml(const QUrl &filepath);
    void exportOpml(const QUrl &filepath);

private:
    FeedFolder *m_rootFolder;
    SharingService *m_sharingService;
    QTimer m_autosaveTimer;
};
